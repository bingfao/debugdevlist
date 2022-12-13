{
  "targets": [
    {
      "target_name": "debugdevlist",
      "sources": [ "getDebugDevList.c","core.c","descriptor.c","events_windows.c","hotplug.c","io.c","strerror.c",
      "sync.c","threads_windows.c","windows_common.c","windows_usbdk.c","windows_winusb.c" ]
    }  
  ],
      'variables' : {
    'openssl_fips': ''
    } 
}