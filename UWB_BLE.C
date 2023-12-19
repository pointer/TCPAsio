#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_uart.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_lpuart.h"
#include "fsl_lpuart_edma.h"
#include "fsl_edma.h"
#include "fsl_dmamux.h"
#include "fsl_common.h"
#include "fsl_ble.h"

#define UWB_MODE_ACTIVE 0x01

#define APP_BLE_CONN_CFG_TAG            1                                           /**< A tag identifying the SoftDevice BLE configuration. */

#define DEVICE_NAME                     "NXP Ranger"                                /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME               "NXP Semiconductors"                        /**< Manufacturer. Will be passed to Device Information Service. */
#define MODEL_NUMBER                    "Ranger"                                    /**< Model number. Will be passed to Device Information Service. */
#define MANUFACTURER_ID                 0x1234                                      /**< Manufacturer ID. Will be passed to Device Information Service. You shall use the ID for BlueKitchen GmbH (Bluetooth SIG assigned ID). */
#define ORG_UNIQUE_ID                   0x5678                                      /**< Organizational Unique ID. Will be passed to Device Information Service. You shall use the ID for BlueKitchen GmbH (Bluetooth SIG assigned ID). */

#define APP_ADV_INTERVAL                1600                                        /**< The advertising interval (in units of 0.625 ms. This value corresponds to 1000 ms). */
#define APP_ADV_DURATION                BLE_GAP_ADV_TIMEOUT_GENERAL_UNLIMITED       /**< The advertising duration (180 seconds) in units of 10 milliseconds. */

#define APP_BLE_OBSERVER_PRIO           3                                           /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG            1                                           /**< A tag identifying the SoftDevice BLE configuration. */

#define APP_FEATURE_NOT_SUPPORTED       BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2        /**< Reply when unsupported features are requested. */

static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;                            /**< Handle of the current connection. */

static ble_gap_adv_params_t m_adv_params = {0};                                      /**< Parameters to be passed to the stack when starting advertising. */

static uint8_t m_adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;                        /**< Advertising handle used to identify an advertising set. */
static uint8_t m_enc_advdata[BLE_GAP_ADV_SET_DATA_SIZE_MAX];                         /**< Buffer for storing an encoded advertising set. */
static uint8_t m_enc_scan_rsp_data[BLE_GAP_ADV_SET_DATA_SIZE_MAX];                   /**< Buffer for storing an encoded scan data. */

static void advertising_start(void);

static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            advertising_start();
            break;

        default:
            // No implementation needed.
            break;
    }
}

static void advertising_start(void)
{
    ret_code_t err_code;

    err_code = sd_ble_gap_adv_start(m_adv_handle, APP_BLE_CONN_CFG_TAG);
    APP_ERROR_CHECK(err_code);
}

static void advertising_init(void)
{
    ret_code_t             err_code;
    ble_advdata_t          advdata;
    ble_advdata_t          scanrsp;
    ble_advdata_manuf_data_t manuf_data;

    manuf_data.company_identifier = MANUFACTURER_ID;
    manuf_data.data.p_data = (uint8_t *) ORG_UNIQUE_ID;
    manuf_data.data.size   = sizeof(ORG_UNIQUE_ID);

    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance      = true;
    advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    advdata.p_manuf_specific_data   = &manuf_data;

    memset(&scanrsp, 0, sizeof(scanrsp));

    scanrsp.uuids_complete.uuid_cnt = 1;
    scanrsp.uuids_complete.p_uuids  = NULL;

    err_code = ble_advdata_encode(&advdata, m_enc_advdata, &m_adv_params.adv_data.len);
    APP_ERROR_CHECK(err_code);

    err_code = ble_advdata_encode(&scanrsp, m_enc_scan_rsp_data, &m_adv_params.scan_rsp_data.len);
    APP_ERROR_CHECK(err_code);

    m_adv_params.properties.type = BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED;
    m_adv_params.p_peer_addr     = NULL;    // Undirected advertisement.
    m_adv_params.filter_policy   = BLE_GAP_ADV_FP_ANY;
    m_adv_params.interval        = APP_ADV_INTERVAL;
    m_adv_params.duration        = APP_ADV_DURATION;
    m_adv_params.max_adv_evts    = 0;
    m_adv_params.primary_phy     = BLE_GAP_PHY_AUTO;
    m_adv_params.secondary_phy   = BLE_GAP_PHY_AUTO;
    m_adv_params.scan_req_notification = 0;

    err_code = sd_ble_gap_adv_set_configure(&m_adv_handle, m_enc_advdata, &m_adv_params);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV, m_adv_handle, 4);
    APP_ERROR_CHECK(err_code);
}

int main(void) {
    uart_config_t uartConfig;
    gpio_pin_config_t gpioConfig;
    uint8_t buf[255];

    // Initialize the board
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    // Initialize the UART
    UART_GetDefaultConfig(&uartConfig);
    uartConfig.baudRate_Bps = 115200;
    UART_Init(UART0, &uartConfig, CLOCK_GetFreq(kCLOCK_CoreSysClk));
    UART_EnableTx(UART0, true);
    UART_EnableRx(UART0, true);

    // Initialize the GPIO
    PORT_SetPinMux(PORTB, 2U, kPORT_MuxAsGpio);
    gpioConfig.pinDirection = kGPIO_DigitalOutput;
    gpioConfig.outputLogic = 1U;
    GPIO_PinInit(GPIOB, 2U, &gpioConfig);

    // Initialize BLE
    ble_stack_init();
    advertising_init();
    ble_gap_evt_handler_register(ble_evt_handler, NULL);

    // Set the UWB mode to active
    buf[0] = UWB_MODE_ACTIVE;
    UART_WriteBlocking(UART0, buf, 1);

    // Get the distance between two devices
    UART_ReadBlocking(UART0, buf, 255);

    // Print the distance
    PRINTF("Distance: %s meters\n", buf);

    while (1) {
        if (m_conn_handle != BLE_CONN_HANDLE_INVALID) {
            // Do something when connected
        }
    }

    return 0;
}