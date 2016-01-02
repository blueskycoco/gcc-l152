#include "lusb0_usb.h"
#include <stdio.h>
#include <time.h> 
#define MY_VID 0x0483
#define MY_PID 0x5740

// Device configuration and interface id.
#define MY_CONFIG 1
#define MY_INTF 4
//#define TEST_ASYNC

// Device endpoint(s)
#define EP_IN 0x81
#define EP_OUT 0x01
int intf=1;
// Device of bytes to transfer.
#define BUF_SIZE 16384

usb_dev_handle *open_dev(void);

static int transfer_bulk_async(usb_dev_handle *dev,
                               int ep,
                               char *bytes,
                               int size,
                               int timeout);

usb_dev_handle *open_dev()
{
    struct usb_bus *bus;
    struct usb_device *dev;

    for (bus = usb_get_busses(); bus; bus = bus->next)
    {
        for (dev = bus->devices; dev; dev = dev->next)
        {
			printf("filename %s\n",dev->filename);
            if (dev->descriptor.idVendor == MY_VID
                    && dev->descriptor.idProduct == MY_PID
					&& dev->config->interface->altsetting->bInterfaceNumber==intf)
            {
                return usb_open(dev);
            }
        }
    }
    return NULL;
}
void TcharToChar (const TCHAR * tchar, char ** _char)  
{  
    int iLength ;  
	//?????????   
	iLength = WideCharToMultiByte(CP_ACP, 0, (const WCHAR *)tchar, -1, NULL, 0, NULL, NULL);  
	//printf("iLength %d\n",iLength);
	*_char=(char *)malloc(iLength);
	memset(*_char,'\0',iLength);
	//??tchar?????_char    
	WideCharToMultiByte(CP_ACP, 0, (const WCHAR *)tchar, -1, *_char, iLength, NULL, NULL);   
	//printf("%s\n",*_char);
}  
#define W_OP 1
#ifdef TEST_ASYNC
/*
* Read/Write using async transfer functions.
*
* NOTE: This function waits for the transfer to complete essentially making
* it a sync transfer function so it only serves as an example of how one might
* implement async transfers into thier own code.
*/
static int transfer_bulk_async(usb_dev_handle *dev,
                               int ep,
                               char *bytes,
                               int size,
                               int timeout)
{
    // Each async transfer requires it's own context. A transfer
    // context can be re-used.  When no longer needed they must be
    // freed with usb_free_async().
    //
    void* async_context = NULL;
    int ret;

    // Setup the async transfer.  This only needs to be done once
    // for multiple submit/reaps. (more below)
    //
    ret = usb_bulk_setup_async(dev, &async_context, ep);
    if (ret < 0)
    {
        printf("error usb_bulk_setup_async:\n%s\n", usb_strerror());
        goto Done;
    }

    // Submit this transfer.  This function returns immediately and the
    // transfer is on it's way to the device.
    //
    ret = usb_submit_async(async_context, bytes, size);
    if (ret < 0)
    {
        printf("error usb_submit_async:\n%s\n", usb_strerror());
        usb_free_async(&async_context);
        goto Done;
    }

    // Wait for the transfer to complete.  If it doesn't complete in the
    // specified time it is cancelled.  see also usb_reap_async_nocancel().
    //
    ret = usb_reap_async(async_context, timeout);

    // Free the context.
    usb_free_async(&async_context);

Done:
    return ret;
}
#endif
int main(int argc, _TCHAR* argv[])
{
	usb_dev_handle *dev =NULL; /* the device handle */
    char tmp[BUF_SIZE],tmp1[BUF_SIZE],*index,*file_out,*file_in;
    int ret,i;
	time_t ltime1, ltime2;
    void* async_read_context = NULL;
    void* async_write_context = NULL;

    usb_init(); /* initialize the library */
    usb_find_busses(); /* find all busses */
    usb_find_devices(); /* find all connected devices */
	for(i=0;i<BUF_SIZE;i++)
		tmp[i]=i;
	//TcharToChar(argv[1],&index);
	//i=atoi(index);
	printf("open usb device \n");
#if !W_OP
	TcharToChar(argv[1],&file_out);
	HANDLE hFile_out = CreateFile((const CHAR *)(LPCWSTR)(argv[1]),
        GENERIC_WRITE,  
        FILE_SHARE_WRITE,  
        NULL,  
        CREATE_ALWAYS,  
        0,  
        NULL );
	if(hFile_out!=INVALID_HANDLE_VALUE)
		 printf("Create %s ok\n",argv[1]);
	 else
		 printf("Can not open %s\n",argv[1]);
#else
	TcharToChar(argv[1],&file_in);
	 HANDLE hFile_in = CreateFile((const CHAR *)(LPCWSTR)(argv[1]),  
        GENERIC_READ,  
        FILE_SHARE_READ,  
        NULL,  
        OPEN_EXISTING,  
        0,  
        NULL ); 
	  if(hFile_in!=INVALID_HANDLE_VALUE)
		 printf("Open %s ok\n",argv[1]);
	 else
		 printf("Can not open %s\n",argv[1]);
#endif	  
    if (!(dev = open_dev()))
    {
        printf("error opening device: \n%s\n", usb_strerror());
        return 0;
    }
    else
    {
        printf("success: device %04X:%04X opened\n", MY_VID, MY_PID);
    }
	 if (usb_set_configuration(dev, MY_CONFIG) < 0)
    {
        printf("error setting config #%d: %s\n", MY_CONFIG, usb_strerror());
        usb_close(dev);
        return 0;
    }
    else
    {
        printf("success: set configuration #%d\n", MY_CONFIG);
    }
	 
	 if (usb_claim_interface(dev, intf) < 0)
    {
        printf("error claiming interface #%d:\n%s\n", intf, usb_strerror());
        usb_close(dev);
        return 0;
    }
    else
    {
        printf("success: claim_interface #%d\n", intf);
    }

	 ret = usb_control_msg(dev, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
                          14, /* set/get test */
                          2,  /* test type    */
                          intf,  /* interface id */
                          tmp, 1, 1000);
 #if W_OP
	 //dev = open_dev();
	 //usb_set_configuration(dev, MY_CONFIG);
	 //usb_claim_interface(dev, intf);
	 //usb_control_msg(dev, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
     //                     14, /* set/get test */
     //                     2,  /* test type    */
     //                     intf,  /* interface id */
     //                     tmp, 1, 1000);
#endif
	 DWORD nBytes; 
	int bytes_w=0,bytes_r=0;	
	BOOL bResult;  
	time (&ltime1);
	printf("Start Time %ld\n",ltime1);
	while(1)
	{	
#if W_OP
		bResult = ReadFile(hFile_in,tmp,sizeof(tmp),&nBytes,NULL);
		if(nBytes>0)
		{
			//while(check_target_mem(dev)==0)
			//	Sleep(1);
			ret = usb_bulk_write(dev, EP_OUT, tmp, nBytes, 5000);
			if (ret < 0)
			{
				printf("error writing:\n%s\n", usb_strerror());
				printf("success: bulk write %d bytes\n", bytes_w);
			}
			else
			{
				bytes_w=bytes_w+ret;
				// printf("usb %d bulk read %d bytes\n",i, bytes_r);/*, bulk read %d bytes*/
			}
		}
		else
			break;
#else
		#ifdef TEST_ASYNC
		// Running an async read test
		ret = transfer_bulk_async(dev, EP_IN, tmp1, sizeof(tmp1), 5000);
		#else
		ret = usb_bulk_read(dev, EP_IN, tmp1, sizeof(tmp1), 5000);
		#endif
		if (ret < 0)
		{
			printf("error reading:\nep_in=%x %d %s", EP_IN,bytes_r,usb_strerror());
			//printf("success: bulk read %d bytes\n", bytes_r);
		}
		else
		{
			bytes_r=bytes_r+ret;
			WriteFile( hFile_out, tmp1, ret, &nBytes, NULL ); 
			//printf("usb %d bulk read %d bytes %d\n",i, bytes_r,ret);/*, bulk read %d bytes*/
		}
		//printf("usb %d bulk write %d bytes, bulk read %d bytes\n",i, bytes_w,bytes_r);
#endif	
			
	}
	time (&ltime2);
	printf("Stop Time %ld\n",ltime2);
#if !W_OP
	CloseHandle(hFile_out);  	
	if(file_out)
		free(file_out);
#else 
	CloseHandle(hFile_in);
	if(file_in)
		free(file_in);
#endif
	//if(index)
	//	free(index);
	
	usb_release_interface(dev, intf);
	if (dev)
	{
		usb_close(dev);
	}	
    printf("Done.r %d, w %d \r\n",bytes_r,bytes_w);
	if(ltime2-ltime1>0)
		printf("%f MBytes/s\n",((float)bytes_w)/((float)(((ltime2-ltime1)*1024*1024))));
	else
		printf("1.25 Mbytes/s\n");
	return 0;
}


