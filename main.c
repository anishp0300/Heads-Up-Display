
/*REQUIRED HEADER FILES*/

#include <stdint.h>
#include <string.h>
#include <time.h>
#include "nordic_common.h"
#include "nrf.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "app_timer.h"
#include "ble_nus.h"
#include "app_uart.h"
#include "app_util_platform.h"
#include "bsp_btn_ble.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_drv_timer.h"
#include "nrf_delay.h"


#include "nrf_drv_twi.h"
#include "ssd1306.h"
#include "Adafruit_GFX.h"


#if defined (UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined (UARTE_PRESENT)
#include "nrf_uarte.h"
#endif

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_drv_saadc.h"
#include "nrf_drv_ppi.h"

#define APP_BLE_CONN_CFG_TAG            1                                           /**< A tag identifying the SoftDevice BLE configuration. */

#define DEVICE_NAME                     "Avid Auto Tech HUD"                               /**< Name of device. Will be included in the advertising data. */
#define NUS_SERVICE_UUID_TYPE           BLE_UUID_TYPE_VENDOR_BEGIN                  /**< UUID type for the Nordic UART Service (vendor specific). */

#define APP_BLE_OBSERVER_PRIO           3                                           /**< Application's BLE observer priority. You shouldn't need to modify this value. */

#define APP_ADV_INTERVAL                64                                          /**< The advertising interval (in units of 0.625 ms. This value corresponds to 40 ms). */

#define APP_ADV_DURATION                18000                                       /**< The advertising duration (180 seconds) in units of 10 milliseconds. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(20, UNIT_1_25_MS)             /**< Minimum acceptable connection interval (20 ms), Connection interval uses 1.25 ms units. */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(75, UNIT_1_25_MS)             /**< Maximum acceptable connection interval (75 ms), Connection interval uses 1.25 ms units. */
#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds), Supervision Timeout uses 10 ms units. */
#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)                       /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000)                      /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define UART_TX_BUF_SIZE                256                                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                256                                         /**< UART RX buffer size. */

#define IN_ACTIVE_TIMEOUT 10000

BLE_NUS_DEF(m_nus, NRF_SDH_BLE_TOTAL_LINK_COUNT);                                   /**< BLE NUS service instance. */
NRF_BLE_GATT_DEF(m_gatt);                                                           /**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr);                                                             /**< Context for the Queued Write module.*/
BLE_ADVERTISING_DEF(m_advertising);                                                 /**< Advertising module instance. */

//Function declarations
void updateTime(bool);     
void hideScreenSaver();    
void showScreenSaver();
void updateBattery();


//Global Variables
volatile long long mTime = 0;   //For storing current time stamp
volatile long long lastTime = -30000;   //Refresh rate for battery percentage and time
long inActivity = 0;                    //For detecting in-activity
bool dotVisible = true;                 //Blinking the ':' while displaying time, toggle holder
bool isScreenSaver = false;             //Screen Saver State.
char lTime[6];                          //long long 6bytes for time
float batteryVoltage = 2.7;             //batteryVolatge Def Init
bool isConnected = false;               //BLE STATUS


const nrf_drv_timer_t TIME_CLOCK = NRF_DRV_TIMER_INSTANCE(4);       //TIMER FOR COUNTING TIME AND REFRESHING SCREEN

static uint16_t   m_conn_handle          = BLE_CONN_HANDLE_INVALID;                 /**< Handle of the current connection. */
static uint16_t   m_ble_nus_max_data_len = BLE_GATT_ATT_MTU_DEFAULT - 3;            /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */
static ble_uuid_t m_adv_uuids[]          =                                          /**< Universally unique service identifier. */
{
    {BLE_UUID_NUS_SERVICE, NUS_SERVICE_UUID_TYPE}
};


//REQUIRED BITMAPS
const unsigned char callBitmap[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x1f, 
	0x00, 0x00, 0x3f, 0x80, 0x00, 0x3f, 0x80, 0x00, 0x1e, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x1e, 0x00, 
	0x00, 0x0f, 0x00, 0x00, 0x07, 0x00, 0x00, 0x07, 0x80, 0x00, 0x03, 0xc0, 0x00, 0x01, 0xe0, 0x00, 
	0x00, 0xf0, 0x60, 0x00, 0x7c, 0xf8, 0x00, 0x3f, 0xfc, 0x00, 0x1f, 0xfc, 0x00, 0x07, 0xf8, 0x00, 
	0x03, 0xf0, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00
};

const unsigned char batteryBitmap[] = {
	0xff, 0xfe, 0x80, 0x02, 0x80, 0x03, 0x80, 0x03, 0x80, 0x03, 0x80, 0x03, 0x80, 0x02, 0xff, 0xfe
};

const unsigned char avidLogo [] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x07, 0xf0, 0x00, 
	0x00, 0x07, 0xf8, 0x00, 0x00, 0x0f, 0xfe, 0x00, 0x00, 0x0f, 0xfe, 0x00, 0x00, 0x0f, 0xfe, 0x00, 
	0x00, 0x1f, 0xfe, 0x00, 0x00, 0x1f, 0xfc, 0x00, 0x00, 0x3f, 0xf8, 0x00, 0x00, 0x3f, 0xf8, 0x00, 
	0x00, 0x3f, 0xf8, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x7f, 0xbe, 0x00, 
	0x00, 0xff, 0xbf, 0x00, 0x00, 0xfe, 0x3f, 0x00, 0x01, 0xfe, 0x3f, 0x00, 0x01, 0xfe, 0x3f, 0xfc, 
	0x01, 0xf8, 0x3f, 0xf0, 0x01, 0xf8, 0xff, 0xc0, 0x03, 0xff, 0xff, 0xe0, 0x07, 0xff, 0xff, 0xe0, 
	0x07, 0xff, 0xff, 0xf0, 0x7f, 0xff, 0xff, 0xf8, 0xff, 0xff, 0x9f, 0xf8, 0xff, 0xff, 0x9f, 0xf8, 
	0x7f, 0xff, 0x0f, 0xfc, 0x7f, 0xf8, 0x0f, 0xfc, 0x7f, 0xf8, 0x0f, 0xfc, 0x7f, 0xe0, 0x0f, 0xfc, 
	0x7f, 0x00, 0x0f, 0xf0, 0x7e, 0x00, 0x0f, 0xc0, 0x7c, 0x00, 0x07, 0x80, 0x60, 0x00, 0x00, 0x00
};

const unsigned char bleLogo [] = {
	0x00, 0x03, 0xfc, 0x00, 0x00, 0x00, 0x1f, 0xff, 0x80, 0x00, 0x00, 0x3f, 0xff, 0xc0, 0x00, 0x00, 
	0x7f, 0xbf, 0xe0, 0x00, 0x00, 0xff, 0x9f, 0xf0, 0x00, 0x01, 0xff, 0x8f, 0xf8, 0x00, 0x01, 0xff, 
	0x87, 0xf8, 0x00, 0x03, 0xff, 0x83, 0xfc, 0x00, 0x03, 0xff, 0x81, 0xfc, 0x00, 0x03, 0xff, 0x80, 
	0xfe, 0x00, 0x03, 0xe7, 0x88, 0x7e, 0x00, 0x07, 0xc3, 0x8c, 0x3e, 0x00, 0x07, 0xe1, 0x8c, 0x3e, 
	0x00, 0x07, 0xf0, 0x88, 0x7e, 0x00, 0x07, 0xf8, 0x00, 0xfe, 0x00, 0x07, 0xfc, 0x01, 0xfe, 0x00, 
	0x07, 0xfe, 0x03, 0xfe, 0x00, 0x07, 0xff, 0x07, 0xfe, 0x00, 0x07, 0xff, 0x07, 0xfe, 0x00, 0x07, 
	0xfe, 0x03, 0xfe, 0x00, 0x07, 0xfc, 0x01, 0xfe, 0x00, 0x07, 0xf8, 0x00, 0xfe, 0x00, 0x07, 0xf0, 
	0x88, 0x7e, 0x00, 0x07, 0xe1, 0x8c, 0x3e, 0x00, 0x07, 0xc3, 0x8c, 0x3e, 0x00, 0x03, 0xe7, 0x88, 
	0x7e, 0x00, 0x03, 0xff, 0x80, 0xfe, 0x00, 0x03, 0xff, 0x81, 0xfc, 0x00, 0x03, 0xff, 0x83, 0xfc, 
	0x00, 0x01, 0xff, 0x87, 0xf8, 0x00, 0x01, 0xff, 0x8f, 0xf8, 0x00, 0x00, 0xff, 0x9f, 0xf0, 0x00, 
	0x00, 0x7f, 0xbf, 0xe0, 0x00, 0x00, 0x3f, 0xff, 0xc0, 0x00, 0x00, 0x1f, 0xff, 0x80, 0x00, 0x00, 
	0x03, 0xfc, 0x00, 0x00
};

const unsigned char smsIcon [] = {
	0x7f, 0xff, 0xe0, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xf0, 0xff, 
	0xff, 0xf0, 0xf9, 0x99, 0xf0, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xf0, 0xff, 0xff, 
	0xf0, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xe0, 0xe0, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00
};

/**@brief Function for assert macro callback.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyse
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num    Line number of the failing ASSERT call.
 * @param[in] p_file_name File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/**@brief Function for initializing the timer module.
 */
static void timers_init(void)
{
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for the GAP initialization.
 *
 * @details This function will set up all the necessary GAP (Generic Access Profile) parameters of
 *          the device. It also sets the permissions and appearance.
 */
static void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *) DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for handling the data from the Nordic UART Service.
 *
 * @details This function will process the data received from the Nordic UART BLE Service and send
 *          it to the UART module.
 *
 * @param[in] p_evt       Nordic UART Service event.
 */


int16_t writeString(int16_t x , int16_t y , uint8_t *c , uint16_t len , int16_t textSize){
    

    for(uint16_t l = 0 ; l < len ; l++){
      Adafruit_GFX_drawChar(x + (l * 6 * textSize) ,y , *c , WHITE , BLACK , textSize);
       c++;
    }
    
    SSD1306_display();
    return len;
}

int16_t writeStringCenterHorizontal(int16_t y , uint8_t *c , uint16_t len , int16_t textSize){
    uint16_t x = (128 - ((len - 1) * 6 * textSize)) / 2;

    for(uint16_t l = 0 ; l < len ; l++){
      Adafruit_GFX_drawChar(x + (l * 6 * textSize) ,y , *c , WHITE , BLACK , textSize);
       c++;
    }
    
    SSD1306_display();
    return len;
}

void timer_led_event_handler(nrf_timer_event_t event_type, void* p_context){

    switch (event_type)
    {
        case NRF_TIMER_EVENT_COMPARE0:
            mTime+=1;
            
            if(!isScreenSaver){
               inActivity++;
               if(inActivity > IN_ACTIVE_TIMEOUT){
                showScreenSaver();
                inActivity = 0;
               }
            }
            break;

        default:
            //Do nothing.
            break;
    }

}


void showScreenSaver(){
    isScreenSaver = true;
    updateTime(true);
}

void hideScreenSaver(){
    Adafruit_GFX_fillRect(0 , 9 , 128 , 55 , BLACK);
    isScreenSaver = false;
    inActivity = 0;
    updateTime(true);
}

/**@snippet [Handling the data received over BLE] */
static void nus_data_handler(ble_nus_evt_t * p_evt)
{

    


    if (p_evt->type == BLE_NUS_EVT_RX_DATA)
    {

        uint8_t packetType = p_evt->params.rx_data.p_data[0];
        
        int nameLen;

        if(packetType == 0x54){
              long long t = 0;
              t |= (long long)p_evt->params.rx_data.p_data[1];
              t |= (long long)(p_evt->params.rx_data.p_data[2]) << 8;
              t |= (long long)(p_evt->params.rx_data.p_data[3]) << 16;
              t |= (long long)(p_evt->params.rx_data.p_data[4]) << 24;
              t |= (long long)(p_evt->params.rx_data.p_data[5]) << 32;
              t |= (long long)(p_evt->params.rx_data.p_data[6]) << 40;
              t |= (long long)(p_evt->params.rx_data.p_data[7]) << 48;
              t |= (long long)(p_evt->params.rx_data.p_data[8]) << 56;

              mTime = t;
        }else if(packetType == 0x43){
              if(true){
                    char name[p_evt->params.rx_data.length - 1];
                    for(int i = 1 ; i < p_evt->params.rx_data.length ; i++){
                        name[i-1] = (char)p_evt->params.rx_data.p_data[i];
                    }
                    hideScreenSaver();
                    Adafruit_GFX_drawBitmap(52,20 , callBitmap , 24,24 , WHITE);
                    writeString((128 - ((p_evt->params.rx_data.length - 1) * 6)) / 2 ,50, name , p_evt->params.rx_data.length - 1 , 1);
                    SSD1306_display();
              }
        }else if(packetType == 0x4D){
              hideScreenSaver();
                
              uint8_t bmp[72];
              for(int i = 1 ; i <= 72 ; i++){
                  bmp[i - 1] = p_evt->params.rx_data.p_data[i];
              }
              Adafruit_GFX_drawBitmap(52 , 20 , bmp , 24 , 24 , WHITE);
              char text[p_evt->params.rx_data.length - 73];
              for(int i = 73 ; i < p_evt->params.rx_data.length ; i++){
                   text[i-73] = (char)p_evt->params.rx_data.p_data[i];
               }
              SSD1306_display();
              writeString(34 ,48, "     " , 5 , 2);
              writeString((128 - ((p_evt->params.rx_data.length - 73) * 6 * 2)) / 2 , 48, text , p_evt->params.rx_data.length - 73 , 2);
              SSD1306_display();
        }else if(packetType == 0x53){
              hideScreenSaver();
              Adafruit_GFX_drawBitmap(54 , 24 , smsIcon , 20 , 16 , WHITE);
              int nameLen = 0;

              for(int x = 0 ; p_evt->params.rx_data.p_data[x] != '\0' ; x++){
                  nameLen++;
              }

              char name[nameLen];

              for(int x = 0 ; x < nameLen ; x++){
                  name[x] = (char)p_evt->params.rx_data.p_data[x + 1];
              }

              int msgLen = p_evt->params.rx_data.length - nameLen;

              char msg[msgLen];
              for(int x = 0 ; x < msgLen ; x++){
                  msg[x] = (char)p_evt->params.rx_data.p_data[x + nameLen];
              }

              writeStringCenterHorizontal(44 , name , nameLen , 1);
              SSD1306_display();

              nrf_delay_ms(2000);

              Adafruit_GFX_fillRect(0 , 9 , 128 , 55 , BLACK);
              Adafruit_GFX_setCursor(0 , 22);
              Adafruit_GFX_setTextColor(WHITE , BLACK);
              Adafruit_GFX_setTextWrap(true);
              Adafruit_GFX_setTextSize(1);
              for(int x = 0 ; x < 84 ; x++){
                  Adafruit_GFX_write(msg[x]);
              }
              //writeString(0 , 35 , msg , msgLen , 1);
              SSD1306_display();

              nrf_delay_ms(5000);

              Adafruit_GFX_fillRect(0 , 9 , 128 , 55 , BLACK);
              Adafruit_GFX_setCursor(0 , 22);
              Adafruit_GFX_setTextColor(WHITE , BLACK);
              Adafruit_GFX_setTextWrap(true);
              Adafruit_GFX_setTextSize(1);
              for(int x = 84 ; x < msgLen ; x++){
                  Adafruit_GFX_write(msg[x]);
              }
              //writeString(0 , 35 , msg , msgLen , 1);
              SSD1306_display();


        }else if(packetType == 0x4E){
              hideScreenSaver();
                
              uint8_t bmp[72];
              for(int i = 1 ; i <= 72 ; i++){
                  bmp[i - 1] = p_evt->params.rx_data.p_data[i];
              }
              Adafruit_GFX_drawBitmap(52 , 20 , bmp , 24 , 24 , WHITE);
              char text[p_evt->params.rx_data.length - 73];
              for(int i = 73 ; i < p_evt->params.rx_data.length ; i++){
                   text[i-73] = (char)p_evt->params.rx_data.p_data[i];
               }
              SSD1306_display();
              writeString(34 ,48, "     " , 5 , 2);
              writeString((128 - ((p_evt->params.rx_data.length - 73) * 6 * 2)) / 2 , 48, text , p_evt->params.rx_data.length - 73 , 2);
              SSD1306_display();
        }
        

        uint32_t err_code;

        NRF_LOG_DEBUG("Received data from BLE NUS. Writing data on UART.");
        NRF_LOG_HEXDUMP_DEBUG(p_evt->params.rx_data.p_data, p_evt->params.rx_data.length);

    }

}


/**@snippet [Handling the data received over BLE] */


/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void)
{
    uint32_t           err_code;
    ble_nus_init_t     nus_init;
    nrf_ble_qwr_init_t qwr_init = {0};

    // Initialize Queued Write Module.
    qwr_init.error_handler = nrf_qwr_error_handler;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);

    // Initialize NUS.
    memset(&nus_init, 0, sizeof(nus_init));

    nus_init.data_handler = nus_data_handler;

    err_code = ble_nus_init(&m_nus, &nus_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling an event from the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module
 *          which are passed to the application.
 *
 * @note All this function does is to disconnect. This could have been done by simply setting
 *       the disconnect_on_fail config parameter, but instead we use the event handler
 *       mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    uint32_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for handling errors from the Connection Parameters module.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
static void sleep_mode_enter(void)
{
    uint32_t err_code = bsp_indication_set(BSP_INDICATE_IDLE);
    APP_ERROR_CHECK(err_code);

    // Prepare wakeup buttons.
    err_code = bsp_btn_ble_sleep_mode_prepare();
    APP_ERROR_CHECK(err_code);

    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    uint32_t err_code;

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
            APP_ERROR_CHECK(err_code);
            break;
        default:
            break;
    }
}


/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    uint32_t err_code;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("Connected");
            isConnected = true;
            err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
            APP_ERROR_CHECK(err_code);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            isConnected = false;
            NRF_LOG_INFO("Disconnected");
            // LED indication will be changed when advertising starts.
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            NRF_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            // Pairing not supported
            err_code = sd_ble_gap_sec_params_reply(m_conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            // No system attributes have been stored.
            err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for the SoftDevice initialization.
 *
 * @details This function initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}


/**@brief Function for handling events from the GATT library. */
void gatt_evt_handler(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt)
{
    if ((m_conn_handle == p_evt->conn_handle) && (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED))
    {
        m_ble_nus_max_data_len = p_evt->params.att_mtu_effective - OPCODE_LENGTH - HANDLE_LENGTH;
        NRF_LOG_INFO("Data len is set to 0x%X(%d)", m_ble_nus_max_data_len, m_ble_nus_max_data_len);
    }
    NRF_LOG_DEBUG("ATT MTU exchange completed. central 0x%x peripheral 0x%x",
                  p_gatt->att_mtu_desired_central,
                  p_gatt->att_mtu_desired_periph);
}


/**@brief Function for initializing the GATT library. */
void gatt_init(void)
{
    ret_code_t err_code;

    err_code = nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_gatt_att_mtu_periph_set(&m_gatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated by button press.
 */
void bsp_event_handler(bsp_event_t event)
{
    uint32_t err_code;
    switch (event)
    {
        case BSP_EVENT_SLEEP:
            sleep_mode_enter();
            break;

        case BSP_EVENT_DISCONNECT:
            err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            if (err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
            break;

        case BSP_EVENT_WHITELIST_OFF:
            if (m_conn_handle == BLE_CONN_HANDLE_INVALID)
            {
                err_code = ble_advertising_restart_without_whitelist(&m_advertising);
                if (err_code != NRF_ERROR_INVALID_STATE)
                {
                    APP_ERROR_CHECK(err_code);
                }
            }
            break;

        default:
            break;
    }
}


/**@brief   Function for handling app_uart events.
 *
 * @details This function will receive a single character from the app_uart module and append it to
 *          a string. The string will be be sent over BLE when the last character received was a
 *          'new line' '\n' (hex 0x0A) or if the string has reached the maximum data length.
 */
/**@snippet [Handling the data received over UART] */
void uart_event_handle(app_uart_evt_t * p_event)
{
    static uint8_t data_array[BLE_NUS_MAX_DATA_LEN];
    static uint8_t index = 0;
    uint32_t       err_code;
    

    switch (p_event->evt_type)
    {
        case APP_UART_DATA_READY:
            UNUSED_VARIABLE(app_uart_get(&data_array[index]));
            index++;

            if ((data_array[index - 1] == '\n') ||
                (data_array[index - 1] == '\r') ||
                (index >= m_ble_nus_max_data_len))
            {
                if (index > 1)
                {
                    NRF_LOG_DEBUG("Ready to send data over BLE NUS");
                    NRF_LOG_HEXDUMP_DEBUG(data_array, index);

                    do
                    {
                        uint16_t length = (uint16_t)index;
                        err_code = ble_nus_data_send(&m_nus, data_array, &length, m_conn_handle);
                        if ((err_code != NRF_ERROR_INVALID_STATE) &&
                            (err_code != NRF_ERROR_RESOURCES) &&
                            (err_code != NRF_ERROR_NOT_FOUND))
                        {
                            APP_ERROR_CHECK(err_code);
                        }
                    } while (err_code == NRF_ERROR_RESOURCES);
                }

                index = 0;
            }
            break;

        case APP_UART_COMMUNICATION_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_communication);
            break;

        case APP_UART_FIFO_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_code);
            break;

        default:
            break;
    }
}
/**@snippet [Handling the data received over UART] */


/**@brief  Function for initializing the UART module.
 */
/**@snippet [UART Initialization] */
static void uart_init(void)
{
    uint32_t                     err_code;
    app_uart_comm_params_t const comm_params =
    {
        .rx_pin_no    = RX_PIN_NUMBER,
        .tx_pin_no    = TX_PIN_NUMBER,
        .rts_pin_no   = RTS_PIN_NUMBER,
        .cts_pin_no   = CTS_PIN_NUMBER,
        .flow_control = APP_UART_FLOW_CONTROL_DISABLED,
        .use_parity   = false,
#if defined (UART_PRESENT)
        .baud_rate    = NRF_UART_BAUDRATE_115200
#else
        .baud_rate    = NRF_UARTE_BAUDRATE_115200
#endif
    };

    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_event_handle,
                       APP_IRQ_PRIORITY_LOWEST,
                       err_code);
    APP_ERROR_CHECK(err_code);
}

static void advertising_init(void)
{
    uint32_t               err_code;
    ble_advertising_init_t init;

    memset(&init, 0, sizeof(init));

    init.advdata.name_type          = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance = false;
    init.advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;

    init.srdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.srdata.uuids_complete.p_uuids  = m_adv_uuids;

    init.config.ble_adv_fast_enabled  = true;
    init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
    init.config.ble_adv_fast_timeout  = APP_ADV_DURATION;
    init.evt_handler = on_adv_evt;

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}


/**@brief Function for initializing buttons and leds.
 *
 * @param[out] p_erase_bonds  Will be true if the clear bonding button was pressed to wake the application up.
 */
static void buttons_leds_init(bool * p_erase_bonds)
{
    bsp_event_t startup_event;

    uint32_t err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS, bsp_event_handler);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_btn_ble_init(NULL, &startup_event);
    APP_ERROR_CHECK(err_code);

    *p_erase_bonds = (startup_event == BSP_EVENT_CLEAR_BONDING_DATA);
}


/**@brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}




/**@brief Function for starting advertising.
 */
static void advertising_start(void)
{
    uint32_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(err_code);
}


const nrf_drv_twi_t m_twi_master = NRF_DRV_TWI_INSTANCE(0);

void twi_init (void)
{
    ret_code_t err_code;

    const nrf_drv_twi_config_t twi_sensors_config = {
       .scl                = 31,  
       .sda                = ARDUINO_SDA_PIN,
       .frequency          = NRF_TWI_FREQ_400K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH
    };

    //err_code = nrf_drv_twi_init(&m_twi_lis2dh12, &twi_lis2dh12_config, twi_handler, NULL);
    err_code = nrf_drv_twi_init(&m_twi_master, &twi_sensors_config, NULL, NULL);        // twi in blocking mode.
    //APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_twi_master);
}


#define SSD1306_CONFIG_SCL_PIN      27
#define SSD1306_CONFIG_SDA_PIN      26


void updateTime(bool force){

  if((mTime - lastTime) > 1000){
      lastTime = mTime;
      dotVisible = !dotVisible;
      if(isScreenSaver){
          writeString(58,20, dotVisible ? ":" : " " , 1 , 2);
      }else{
          writeString(106,1, dotVisible ? ":" : " " , 1 , 1);
      }
      
      SSD1306_display();
  }

  time_t rawtime = (mTime / 1000);
  struct tm  ts;
  char       date[11];
  char       mTime[6];

  ts = *localtime(&rawtime);
  strftime(date, sizeof(date), "%d-%m-%Y", &ts);
  strftime(mTime, sizeof(mTime), "%H:%M", &ts);

  if(strcmp(lTime , mTime) == 0 & !force){
      return;
  }
  strcpy(lTime , mTime);
  
  if(isScreenSaver){
     Adafruit_GFX_fillRect(0 , 9 , 128 , 55 , BLACK);
     Adafruit_GFX_fillRect(17 , 0 , 111 , 9 , BLACK);
     writeString(33 ,39, date , 10 , 1);
     writeString(34,20, mTime , 5 , 2);
  }else{
     writeString(94,1, mTime , 5 , 1);
  }
  

  SSD1306_display();

}


//=========================SAADC=================================

// Samples are needed to be stored in a buffer, we define the length here
#define SAMPLE_BUFFER_LEN 5 

// Save the samples in double buffer which is  a two dimentional array
static nrf_saadc_value_t m_buffer_pool[2][SAMPLE_BUFFER_LEN]; 

// Handle the events once the samples are received in the buffer
void saadc_callback_handler(nrf_drv_saadc_evt_t const * p_event)
{

    if(p_event -> type == NRFX_SAADC_EVT_DONE) // check if the sampling is done and we are ready to take these samples for processing
    {
      ret_code_t err_code; // a variable to hold errors code

// A function to take the samples (which are in the buffer in the form of 2's complement), and convert them to 16-bit interger values
      err_code = nrfx_saadc_buffer_convert(p_event->data.done.p_buffer, SAMPLE_BUFFER_LEN);
      APP_ERROR_CHECK(err_code); // check for errors

// a simple variable for loop
      int i;

// simple log message to show some event occured
      NRF_LOG_INFO("ADC Event Occurred!!");

      int avg = 0;

// For loop is used to read and process each variable in the buffer, if the buffer size is 1, we don't need for loop
      for(i = 0; i<SAMPLE_BUFFER_LEN; i++)
      {
        NRF_LOG_INFO("Sample Value Read: %d", p_event->data.done.p_buffer[i]); // read the variable and print it

// Perform some calculations to convert this value back to the voltage
        avg += p_event->data.done.p_buffer[i];
	
        }

        avg = avg / SAMPLE_BUFFER_LEN;

        float fact = 0.00350438f;
        batteryVoltage = ((float)avg) * fact;

    }
}



void saadc_init(void)
{
  ret_code_t err_code; // variable to store the error code

// Create a struct to hold the default configurations which will be used to initialize the adc module.
// make sure to use the right pins
  nrf_saadc_channel_config_t channel_config = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN0);

// Initialize the adc module Null is for configurations, they can be configured via CMSIS Configuration wizard so we don't need to pass anything here
  err_code = nrf_drv_saadc_init(NULL, saadc_callback_handler);
  APP_ERROR_CHECK(err_code);

// allocate the channel along with the configurations
  err_code = nrfx_saadc_channel_init(0, &channel_config);
  APP_ERROR_CHECK(err_code);

// allocate first buffer where the values will be stored once sampled
  err_code = nrfx_saadc_buffer_convert(m_buffer_pool[0], SAMPLE_BUFFER_LEN);
  APP_ERROR_CHECK(err_code);

// allocate second buffer where the values will be stored if overwritten on first before reading
  err_code = nrfx_saadc_buffer_convert(m_buffer_pool[1], SAMPLE_BUFFER_LEN);
  APP_ERROR_CHECK(err_code);

}

//===============================================================

/**@brief Application main function.
 */
int main(void)
{
    bool erase_bonds;

    saadc_init();

    twi_init();
    SSD1306_begin(SSD1306_SWITCHCAPVCC, 0x3C, false);
    Adafruit_GFX_init(SSD1306_LCDWIDTH, SSD1306_LCDHEIGHT, SSD1306_drawPixel);
    SSD1306_clearDisplay();

    Adafruit_GFX_drawBitmap(49, 10 , avidLogo , 30 , 36 , WHITE);
    writeString(20 , 52 , "AVID AUTO TECH" , 15 , 1);
    

    SSD1306_display();

    uint32_t time_ms = 1; //Time(in miliseconds) between consecutive compare events.
    uint32_t time_ticks;
    uint32_t err_code = NRF_SUCCESS;

    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    err_code = nrf_drv_timer_init(&TIME_CLOCK, &timer_cfg, timer_led_event_handler);
    APP_ERROR_CHECK(err_code);

    time_ticks = nrf_drv_timer_ms_to_ticks(&TIME_CLOCK, time_ms);

    nrf_drv_timer_extended_compare(
         &TIME_CLOCK, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

    nrf_drv_timer_enable(&TIME_CLOCK);

    // Initialize.
    uart_init();
    log_init();
    timers_init();
    buttons_leds_init(&erase_bonds);
    //power_management_init();
    ble_stack_init();
    gap_params_init();
    gatt_init();
    services_init();
    advertising_init();
    conn_params_init();

    // Start execution.
    printf("\r\nUART started.\r\n");
    NRF_LOG_INFO("Debug logging for UART over RTT started.");
    
    advertising_start();

    nrf_delay_ms(2000);

    SSD1306_clearDisplay();

    writeStringCenterHorizontal(15 , "WELCOME!" , 8 , 1);
    writeStringCenterHorizontal(40 , "RIDER" , 5 , 2);

    SSD1306_display();
    nrf_delay_ms(1500);

    SSD1306_clearDisplay();

    // Enter main loop.
    for (;;)
    {
        __WFI();

        if(!isConnected){
            while(!isConnected){
                SSD1306_clearDisplay();
                Adafruit_GFX_drawBitmap(46 , 14 , bleLogo , 36 , 36 , WHITE);
                SSD1306_display();
                nrf_delay_ms(500);

                SSD1306_clearDisplay();
                SSD1306_display();
                nrf_delay_ms(500);
            }
            
            updateTime(true);
            
        }
        updateTime(false);
        nrfx_saadc_sample();
        updateBattery();
        SSD1306_display();
        nrf_delay_ms(1000);
        
        
        //idle_state_handle();
    }
}

void updateBattery(){

  Adafruit_GFX_fillRect(0 , 0 , 16 , 8 , BLACK);
  Adafruit_GFX_drawBitmap(0 , 0 , batteryBitmap , 16 , 8 , WHITE);

  int max = (int)((batteryVoltage / 2.66f) * 13.0f);
  max = max > 13 ? 13 : max;

  for(int i = 2 ; i < max ; i++){
      SSD1306_drawPixel(i , 2 , WHITE);
      SSD1306_drawPixel(i , 3 , WHITE);
      SSD1306_drawPixel(i , 4 , WHITE);
      SSD1306_drawPixel(i , 5 , WHITE);
  }
  

}

/**
 * @}
 */
