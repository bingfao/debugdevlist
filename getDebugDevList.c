#include <node_api.h>
#include "libusb.h"
#include <stdio.h>




void listAllUsbDev(napi_env env, napi_value *pArr)
{
    struct libusb_context *usb_ctx = NULL;
    if (libusb_init(&usb_ctx) < 0)
    {
        printf("libusb_init() failed");
        return;
    }
    struct libusb_device **devs = NULL;
    int i = 0;
    if (libusb_get_device_list(usb_ctx, &devs) < 0)
    {
        printf("libusb_get_device_list() failed");
        libusb_exit(usb_ctx);
        return;
    }
    struct libusb_device *dev = NULL;
    struct libusb_device_descriptor dev_desc;
    const int nbuflen = 64;
    char szbuf[64] = {0};
    uint32_t index = 0;
    while ((dev = devs[i++]) != NULL)
    {
        if (libusb_get_device_descriptor(dev, &dev_desc) == 0)
        {
            // libusb_get_string_descriptor_ascii(dev, dev_desc.iManufacturer, szbuf, nbuflen);
            if (dev_desc.bDeviceClass != 0x09)
            // {
            //     printf("Hub  vid: 0x%04X, pid: 0x%04X.\n", dev_desc.idVendor, dev_desc.idProduct);
            // }
            // else
            {
                napi_value devobj;
                napi_status sta = napi_create_object(env, &devobj);
                printf("vid: 0x%04X, pid: 0x%04X, serialNum: 0x%02X, devClass: 0x%04X.\n", dev_desc.idVendor, dev_desc.idProduct, dev_desc.iSerialNumber, dev_desc.bDeviceClass);
                int devbus = libusb_get_bus_number(dev);
                // int devport = libusb_get_port_number(dev);
                uint8_t nPort[7] = {0};
                char szPort[18] = {0};
                int nrt = libusb_get_port_numbers(dev, nPort, 7);
                if (nrt > 0)
                {
                    printf("devPort:  %d-", devbus);
                    szPort[0] = 0x30 + devbus;
                    szPort[1] = '-';
                    for (int i = 0, j = 2; i < nrt; ++i)
                    {
                        if (i)
                        {
                            printf(".%u", nPort[i]);
                            szPort[j++] = '.';
                        }
                        else
                            printf("%u", nPort[i]);
                        // szPort[j++] = nPort[i];
                        sprintf_s(szPort + j, 3, "%u", nPort[i]);
                        ++j;
                    }
                    printf("\n");
                    printf("szPort is: %s.\n", szPort);
                }
                if (sta == napi_ok)
                {
                    napi_value val;
                    sta = napi_create_int32(env, dev_desc.idVendor, &val);
                    sta = napi_set_named_property(env, devobj, "vid", val);
                    sta = napi_create_int32(env, dev_desc.idProduct, &val);
                    sta = napi_set_named_property(env, devobj, "pid", val);
                    sta = napi_create_string_utf8(env, szPort, strlen(szPort), &val);
                    sta = napi_set_named_property(env, devobj, "port", val);
                    sta = napi_set_element(env, *pArr, index++, devobj);
                }
            }
        }
    }
    libusb_exit(usb_ctx);
}

static napi_value getDebugDevList(napi_env env, napi_callback_info info)
{
    napi_status status;
    napi_value arr;
    status = napi_create_array(env, &arr);
    if (status == napi_ok)
    {
        // status = napi_create_string_utf8(env, "world", 5, &world);
        listAllUsbDev(env, &arr);
    }
    return arr;
}

#define DECLARE_NAPI_METHOD(name, func)         \
    {                                           \
        name, 0, func, 0, 0, 0, napi_default, 0 \
    }

static napi_value Init(napi_env env, napi_value exports)
{
    napi_status status;
    napi_property_descriptor desc = DECLARE_NAPI_METHOD("getDebugDevList", getDebugDevList);
    status = napi_define_properties(env, exports, 1, &desc);
    // assert(status == napi_ok);
    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)