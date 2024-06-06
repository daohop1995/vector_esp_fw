///
/// @file bble.c
/// @author Anuj Pathak (anuj@biii.in)
/// @brief
/// @version 0.1
/// @date 2023-12-09
///
/// @copyright Copyright (c) 2023
///
///
#include "bble.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "nvs_flash.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "console/console.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "errno.h"

static const char *TAG = "mBLE";

#define ERROR_LOG_AND_CHECK(s)           \
    {                                    \
        int rc = s;                      \
        if (rc) {                        \
            ESP_LOGE(TAG, "Err %d", rc); \
            assert(rc == 0);             \
        }                                \
    }

static const ble_uuid128_t VECTOR_SERVICE_UUID128 =
    BLE_UUID128_INIT(0xf0, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80,
                     0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

static const ble_uuid128_t VECTOR_PIPE_CHAR_UUID128 =
    BLE_UUID128_INIT(0xf0, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80,
                     0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

static struct {
    on_bble_data_write_f on_data;
    uint8_t ble_addr_type;
    uint16_t conn_handle;
    uint16_t attr_handle;
    uint8_t data_buf[256];
} _stm = {
    .conn_handle = 0xFFFF,
    .attr_handle = 0xFFFF,
};

static int ble_gap_event(struct ble_gap_event *event, void *arg);

static void ble_connectable_advertise(void)
{
    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields fields;
    const char *name;
    int rc;

    memset(&fields, 0, sizeof fields);
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;

    name = ble_svc_gap_device_name();
    fields.name = (uint8_t *)name;
    fields.name_len = strlen(name);
    fields.name_is_complete = 1;

    rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error setting advertisement data; rc=%d\n", rc);
        return;
    }

    memset(&fields, 0, sizeof fields);
    fields.uuids128 = &VECTOR_SERVICE_UUID128;
    fields.num_uuids128 = 1;
    fields.uuids16_is_complete = 1;

    rc = ble_gap_adv_rsp_set_fields(&fields);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error setting scan rsp data; rc=%d\n", rc);
        return;
    }

#if 0
    static uint8_t own_addr_type = BLE_OWN_ADDR_RANDOM;
    ble_addr_t addr;
    /* Use NRPA */
    rc = ble_hs_id_gen_rnd(1, &addr);
    assert(rc == 0);
    rc = ble_hs_id_set_rnd(addr.val);
    assert(rc == 0);
#else
    static uint8_t own_addr_type = BLE_OWN_ADDR_PUBLIC;
#endif

    /* Begin advertising. */
    memset(&adv_params, 0, sizeof adv_params);
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    adv_params.itvl_min = BLE_GAP_ADV_ITVL_MS(100);
    adv_params.itvl_max = BLE_GAP_ADV_ITVL_MS(200);
    rc = ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER,
                           &adv_params, ble_gap_event, NULL);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error enabling advertisement; rc=%d\n", rc);
        return;
    }
}

static int device_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    uint16_t len = os_mbuf_len(ctxt->om);
    if (len < sizeof(_stm.data_buf)) {
        os_mbuf_copydata(ctxt->om, 0, len, &_stm.data_buf);
        ESP_LOGI(TAG, "receive data from client");
        ESP_LOG_BUFFER_HEX(TAG, (void *)&_stm.data_buf, len);
        _stm.on_data(_stm.data_buf, len);
    }
    return 0;
}

static const struct ble_gatt_svc_def gatt_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &VECTOR_SERVICE_UUID128.u, // Define UUID for device type
        .characteristics = (struct ble_gatt_chr_def[]){
            {
                .uuid = &VECTOR_PIPE_CHAR_UUID128.u, // Define UUID for reading
                .flags = BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_WRITE_NO_RSP | BLE_GATT_CHR_F_WRITE,
                .access_cb = device_write,
                .val_handle = &_stm.attr_handle,
            },
            {0},
        },
    },
    {0},
};

static int ble_gap_event(struct ble_gap_event *event, void *arg)
{
    // struct ble_hs_adv_fields fields;
    switch (event->type) {
    // Advertise if connected
    case BLE_GAP_EVENT_CONNECT:
        ESP_LOGI(TAG, "BLE GAP EVENT CONNECT %s", event->connect.status == 0 ? "OK!" : "FAILED!");
        if (event->connect.status == 0) {
            _stm.conn_handle = event->connect.conn_handle;
        } else {
            _stm.conn_handle = 0xFFFF;
            // ble_connectable_advertise();
        }
        break;
    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI(TAG, "BLE GAP EVENT DISCONNECT %d", event->disconnect.reason);
        /* Connection terminated; resume advertising */
        ble_connectable_advertise();
        _stm.conn_handle = 0xFFFF;
        break;
    // Advertise again after completion of the event
    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI(TAG, "BLE GAP EVENT ADV COMPLETE");
        ble_connectable_advertise();
        break;

    case BLE_GAP_EVENT_DISC_COMPLETE:
        ESP_LOGI(TAG, "BLE GAP DISC COMPLETE EVENT reason=%d\n",
                 event->disc_complete.reason);
        return 0;

    default:
        break;
    }
    return 0;
}

static void ble_multi_adv_on_sync(void)
{
    int rc;
    rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);
    /* Figure out address to use while advertising (no privacy for now) */
    rc = ble_hs_id_infer_auto(0, &_stm.ble_addr_type);
    if (rc != 0) {
        ESP_LOGE(TAG, "error determining address type; rc=%d\n", rc);
        return;
    }
    ble_connectable_advertise();
}

void ble_multi_adv_host_task(void *param)
{
    ESP_LOGI(TAG, "BLE Host Task Started");
    /* This function will return only when nimble_port_stop() is executed */
    nimble_port_run();
    nimble_port_freertos_deinit();
}

int bble_init(on_bble_data_write_f on_data)
{
    esp_err_t ret;
    _stm.on_data = on_data;
    ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init nimble %d ", ret);
        return ret;
    }
    //
    {
        /* Initialize the NimBLE host configuration. */
        ble_hs_cfg.sync_cb = ble_multi_adv_on_sync;

        ble_hs_cfg.sm_bonding = 1;
        ble_hs_cfg.sm_mitm = 1;
        ble_hs_cfg.sm_sc = 1;
        ble_hs_cfg.sm_sc = 0;
        /* Enable the appropriate bit masks to make sure the keys
         * that are needed are exchanged
         */
        ble_hs_cfg.sm_our_key_dist = BLE_SM_PAIR_KEY_DIST_ENC;
        ble_hs_cfg.sm_their_key_dist = BLE_SM_PAIR_KEY_DIST_ENC;
        ble_svc_gap_init();             // 4 - Initialize NimBLE configuration - gap service
        ble_svc_gatt_init();            // 4 - Initialize NimBLE configuration - gatt service
        ble_gatts_count_cfg(gatt_svcs); // 4 - Initialize NimBLE configuration - config gatt services
        ble_gatts_add_svcs(gatt_svcs);  // 4 - Initialize NimBLE configuration - queues gatt services.
    }
    //
    {
        uint16_t euid[4] = {0};
        ret = esp_read_mac((uint8_t *)euid, ESP_MAC_BT);
        // ret = esp_read_mac((uint8_t *)euid, ESP_MAC_BASE);
        assert(ret == 0);
        char euid_name[18] = {0};

        sprintf(euid_name,
                "Vector-%04X-%04X",
                euid[0], euid[1]);
        /* Set the default device name. */
        ret = ble_svc_gap_device_name_set(euid_name);
        assert(ret == 0);
    }
    /* XXX Need to have template for store */
    // ble_store_config_init();
    nimble_port_freertos_init(ble_multi_adv_host_task);
    return ret;
}

int bble_send_data(const uint8_t *data, uint16_t data_len)
{
    struct os_mbuf *om;
    int rc = -ENOTCONN;
    if (_stm.conn_handle != 0xFFFF) {
        om = ble_hs_mbuf_from_flat(data, data_len);
        ESP_LOGI(TAG, "Notifying conn=%d", _stm.conn_handle);
        rc = ble_gattc_notify_custom((uint16_t)_stm.conn_handle, _stm.attr_handle, om);
        if (rc != 0) {
            ESP_LOGE(TAG, "error notifying; rc=%d", rc);
        }
    }
    return rc;
}
