/*------------------------------------------------------------------------*/
/*                                                                        */
/*  Copyright (c) 2008 by MESA Imaging SA,                                */
/*                     http://www.mesa-imaging.ch                         */
/*------------------------------------------------------------------------*/
// $Author$
//    $URL$
//    $Rev$
//   $Date$
/*------------------------------------------------------------------------*/
//!\file
//!contains defines, enumerations and classes for SR API

#pragma once
#if !defined(_WIN32) && !defined(MATLAB_PREPROC)
typedef short          __wchar_t;
typedef int            HWND;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
#endif

//!\addtogroup libMesaSR
//!@{

//!Modulation frequency for function SR_SetModulationFrequency()
//!\warning some frequencies are only for internal usage and not released for customer.
enum ModulationFrq {MF_40MHz=0, //!<SR3k: maximal range 3.75m
                    MF_30MHz,   //!<SR3k, SR4k: maximal range 5m
                    MF_21MHz,   //!<SR3k: maximal range 7.14m
                    MF_20MHz,   //!<SR3k: maximal range 7.5m
                    MF_19MHz,   //!<SR3k: maximal range 7.89m
                    MF_60MHz,   //!<SR4k: maximal range 2.5m \intDoc{frequency}
                    MF_15MHz,   //!<SR4k: maximal range 10m \intDoc{frequency}
                    MF_10MHz,   //!<SR4k: maximal range 15m \intDoc{frequency}
                    MF_29MHz,   //!<SR4k: maximal range 5.17m
                    MF_31MHz,   //!<SR4k: maximal range 4.84m
                    MF_14_5MHz,   //!<SR4k: maximal range 10.34m \intDoc{frequency}
                    MF_15_5MHz,   //!<SR4k: maximal range 9.68m \intDoc{frequency}
                    MF_LAST};

//!Enum used in the function SR_SetMode()
//!The suggested default settings are:
//! - SR2,SR3k: AM_COR_FIX_PTRN|AM_MEDIAN
//! - SR4k: AM_COR_FIX_PTRN|AM_CONV_GRAY|AM_DENOISE_ANF
//!These modes are explained in more detail in the User Manual.
//!\warning Some enumerations are for internal usage only and are not recommended for general use.
enum AcquireMode {
  AM_COR_FIX_PTRN=0x01,       //!< turns on fix pattern noise correction <b>this should always be enabled for good distance measurement</b>
  AM_MEDIAN=0x04,             //!< turns on a 3x3 median filter
  AM_TOGGLE_FRQ=0x08,         //!< For sr3k: toggles each frame from 19 to 21 MHz  \intDoc{mode}
  AM_CONV_GRAY=0x10,          //!< Converts the amplitude image by multiplying by the square of the distance, resulting in image more like a conventional camera.  
  AM_SW_ANF=0x20,             //!< Turns on the 7x7 software adaptive neighborhood filter  \intDoc{mode}
  AM_RESERVED0=0x40,          //!< was AM_SR3K_2TAP_PROC  \intDoc{mode}
  AM_RESERVED1=0x80,          //!< was AM_SHORT_RANGE For sr4k: this flag results in more precise coordinate transformations for small distances(<1m) <b>works only for SR_CoordTrfFlt()</b>
  AM_CONF_MAP=0x0100,         //!< For sr4k: process a confidencemap. this map is accesssible with SR_GetImageList
  AM_HW_TRIGGER=0x0200,       //!< For sr4k: Acquisition starts, when a Hardware Trigger is received (AM_SW_TRIGGER must also be set)
  AM_SW_TRIGGER=0x0400,       //!< For sr4k: Light source is only turned on, when an image is requested
  AM_DENOISE_ANF=0x0800,      //!< For sr4k: Turns on the 5x5 hardware adaptive neighborhood filter
  AM_MEDIANCROSS=0x1000,      //!< turns on a 3x3 cross-median  filter (5 values)  \intDoc{mode}
};

//!Argument for image processing block.
//!\image html ImgProcBlk.png
//!samples:
//!\verbatim
//!SR_SetImgProcBlk(dev,IPB_HISTOGRAM,1);
//!SR_SetImgProcBlk(dev,IPB_IN_THRESHOLD_MIN,12000);
//!char kernel[25]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24};
//!SR_SetImgProcBlk(dev,IPB_CONV0,kernel);
//!\endverbatim
//!\sa SR_SetImgProcBlk
//!\warning \intDoc{mode}
enum IPBArg { 
  IPB_SETUP_CONV,         //!< convolution switches. See also: \ref IPConvSetup
  IPB_SETUP_BINARIZE,     //!< Binatize switches. See also: \ref IPBinatizeSetup
  IPB_HISTOGRAM,          //!< histrogram in last line (on/off). Followed by a uchar with value 0 or 1
  IPB_CONV0,              //!< Convolution kernel 0. Followed by a char[25] with the kernel values  
  IPB_CONV1,              //!< Convolution kernel 1. Followed by a char[25] with the kernel values
  IPB_IN_THRESHOLD_MIN,   //!< Input minimal threshold value. followed by a ushort value
  IPB_IN_THRESHOLD_MAX,   //!< Input minimal threshold value. followed by a ushort value 
  IPB_OUT_THRESHOLD_MIN,  //!<  Input minimal threshold value. followed by a ushort value
  IPB_OUT_THRESHOLD_MAX,  //!<  Input minimal threshold value. followed by a ushort value
};

//!Switches for convolution block.
//!Uned in combination with IPB_SETUP_CONV
//!sample: SR_SetImgProcBlk(dev,IPB_SETUP_CONV,(IPCS_SRC_DIST|IPCS_OPP_ADD|IPCS_OPP_ABS)+sensitivity);
//!value is: (Ored IPConvSetup) + sensitivity.
//!sensitivity may have a value from 0 to 16
enum IPConvSetup{ 
  IPCS_SRC_DIST=0x00,  //!< source selector distance or amplitude
  IPCS_SRC_AMPL=0x80,  //!< "
  IPCS_OPP_ADD=0x00,   //!< kernel combination operation
  IPCS_OPP_SUB=0x20,   //!< " 
  IPCS_OPP_ABS=0x10,   //!< absolute value (on/off)
};

//!Switches for binarization blocks.
//!Uned in combination with IPB_SETUP_BINARIZE
//!sample: SR_SetImgProcBlk(dev,IPB_SETUP_BINARIZE,IPBS_BINARIZE_IN|IPBS_BINARIZE_OUT);
//!        SR_SetImgProcBlk(dev,IPB_SETUP_BINARIZE,0);
//!value is: Ored IPBinatizeSetup.
enum IPBinatizeSetup{ 
  IPBS_BINARIZE_IN =0x01,   //!< input  binarization (ON/OFF)
  IPBS_BINARIZE_OUT=0x02,   //!< output binarization (ON/OFF) 
};


#pragma pack(push,r1,4)
//!structure used in \ref SR_GetImageList()
typedef struct _ImgEntry
{
  //!this enum describes the image type received from the function SR_GetImageList()
  //!the images types relevant for the customer are IT_DISTANCE, IT_AMPLITUDE and IT_CONF_MAP
  //!Currently the data type of these images are DT_USHORT but may alter in the future.
  //!The characteristics of the images is affected by the AcquireMode flags, 
  //!e.g. spatial filtering of distance, distance adjustment of Amplitude, presence/absence of Confidence Map.

#ifndef MATLAB_PREPROC

  enum ImgType {
    IT_DISTANCE,  //!< distance image. Values between 0..0xffff (0xffff=maximal range which depends on modulation frequency)
                  //!< the distance image is affected by the #AcquireMode flags. For further information read the User Manual.
    IT_AMPLITUDE, //!< amplitude image. For further information read the User Manual.
    IT_INTENSITY, //!< intensity image (only for SR2).
    IT_TAP0,      //!< tap0 image. \intDoc{ImgType}
    IT_TAP1,      //!< tap1 image. \intDoc{ImgType}
    IT_TAP2,      //!< tap2 image. \intDoc{ImgType}
    IT_TAP3,      //!< tap3 image. \intDoc{ImgType}
    IT_SUM_DIFF , //!< sum_diff image. (T0+T2)-(T1+T3) \intDoc{ImgType}
    IT_CONF_MAP,  //!< confidence map image. Value 0xffff is highest confidence.
    IT_UNDEFINED, //!< any other kind of image
    IT_LAST       //!< useful last entry
  };
  //!this enum describes the data type of the image
  enum DataType {
    DT_NONE,    //!< no data type
    DT_UCHAR,   //!< unsigned char data type
    DT_CHAR,    //!< char data type
    DT_USHORT,  //!< unsigned short data type
    DT_SHORT,   //!< short data type
    DT_UINT,    //!< unsigned int data type
    DT_INT,     //!< int data type
    DT_FLOAT,   //!< float data type
    DT_DOUBLE   //!< double data type
  };
#endif
  enum ImgType  imgType;  //!< image type
  enum DataType dataType; //!< image data type
  void* data;        //!< pointer to memory
  WORD  width;       //!< width of the image
  WORD  height;      //!< height of the image
}ImgEntry;//!<typedef struct _ImgEntry

#pragma pack(pop,r1)

#ifndef MATLAB_PREPROC
#ifndef __cplusplus
struct _CMesaDevice; //forward declaration
//!handle to camera
typedef struct _CMesaDevice *SRCAM;
#else
class CMesaDevice; //forward declaration
typedef CMesaDevice* SRCAM;
#endif
#endif
//!function type used in SR_SetCallback()
//!\param funcData is the user data pointer that was set with SR_SetCallback.
//!\param msg is the received message of type CM_XXX as listed below
//!\param param is a message specific parameter as listed below
//!\param data is a message specific pointer as listed below
//!
//!defines used for the callback function SR_SetCallback() are:\n
//!
//! <HR> \par message: CM_CHANGING and CM_CHANGED
//!CM_CHANGING is sent before modification of register values or filtering functions\n
//!CM_CHANGED is sent after modification of register values or filtering functions\n
//!\par parameters:
//!CP_AFFECTS_BUFFER is set if the buffer and therefore the images pointers will change or changed
//!\par data:
//!no meaning
//!
//! <HR> \par message: CM_MSG_DISPLAY
//!CM_MSG_DISPLAY is sent to display a message
//!\par parameters:
//!the param is a kind ored with a category: \ref SRMsgKind|\ref SRMsgCategory.\n
//!the default handling will display Messages as followed:
//! - MK_DEBUG_STRING OutputDebugString
//! - MK_BOX_INFO open a ASTERISK MessageBox
//! - MK_BOX_WARN open a WARNING MessageBox
//! - MK_BOX_ERR open a HAND MessageBox
//!\par data:
//!the data is a pointer to the string to display
//!
//! <HR> \par message: CM_PROGRESS
//!CM_PROGRESS is sent to display a progress bar window.
//!the default handling will open a window with a progress bar and close this window on the param CP_DONE.
//!\par parameters:
//!the param split in a LOWORD and HIWORD part.\n
//! - LOWORD: indicates that type of progress message. E.g. CP_FLASH_ERASE means that the flash is beeing erased..
//! - HIWORD: indicates the percentage (0-100) of the grogress.
//!
//! <HR>
typedef int (SR_FuncCB)(SRCAM srCam, unsigned int msg, unsigned int param, void* data);

//defines used for the callback function SR_SetCallback
#define CM_MSG_DISPLAY  0x0001 //!< \#define for \ref SR_FuncCB
//------------------------------------------
#define CM_CHANGING     0x0010 //!< \#define for \ref SR_FuncCB
#define CM_CHANGED      0x0011 //!< \#define for \ref SR_FuncCB
//param
#define CP_AFFECTS_BUFFER 0x0001 //!< \#define for \ref SR_FuncCB
//------------------------------------------
#define CM_PROGRESS       0x0020 //!< \#define for \ref SR_FuncCB
//param LOWORD
#define CP_UNKNOWN      0x0000 //!< \#define for \ref SR_FuncCB
#define CP_FLASH_ERASE  0x0001 //!< \#define for \ref SR_FuncCB
#define CP_FLASH_WRITE  0x0002 //!< \#define for \ref SR_FuncCB
#define CP_FLASH_READ   0x0003 //!< \#define for \ref SR_FuncCB
#define CP_FPGA_BOOT    0x0004 //!< \#define for \ref SR_FuncCB
#define CP_CAM_REBOOT   0x0005 //!< \#define for \ref SR_FuncCB
#define CP_DONE         0x0006 //!< \#define for \ref SR_FuncCB
//param HIWORD
//       progress value (0-100)
//------------------------------------------
#define CM_CAM_SELECT   0x0030 //!< \#define for \ref SR_FuncCB
//param LOWORD is CP_... HIWORD not yet used
#define CP_CS_OPENDLG   0x0000 //!< \#define for \ref SR_FuncCB
#define CP_SCAN_CAM     0x0001 //!< \#define for \ref SR_FuncCB
#define CP_FOUND_CAM    0x0002 //!< \#define for \ref SR_FuncCB
#define CP_FIND_DONE    0x0003 //!< \#define for \ref SR_FuncCB
#define CP_CONNECT      0x0004 //!< \#define for \ref SR_FuncCB
#define CP_CS_CLOSEDLG  0x0005 //!< \#define for \ref SR_FuncCB
#define CP_GET_FILENAME 0x0006 //!< \#define for \ref SR_FuncCB
//CP_CS_OPENDLG :Opens the dialog box (user can overwrite)The user loop calls the SCAN_CAM and CONNECT.
//CP_SCAN_CAM   :Scans the cameras (called in user loop, no overwrite, creates a thread)
//CP_FOUND_CAM  :found cam in scan (user can overwrite)
//CP_FIND_DONE  :cam scan done (user can overwrite,called when scan thread finished)
//CP_CONNECT    :connects the camera (called in user loop, no overwrite)
//CP_CS_CLOSEDLG:called when user loops exits, to release resources.(no overwrite)
//
//data is a pointer a CSData structure
//in case of CP_GET_FILENAME, the data pointer is a char[512] buffer for the filename (mem is user owened)
//Camera selection data:
typedef struct _CSCamera
{
  char devStr[256];
  void* reserved; //for internal use: contains a CMesaDeviceFactory::CSCameraLoc structure
  struct _CSCamera *next;
}CSCamera;

//root camera selection data
typedef struct _CSData
{
  HWND _parent;
  SRCAM srCam;       //resulting camera object pointer
  CSCamera* camList; // chainedCameraList
}CSData;


//!Message Kind: enum for function \ref SR_FuncCB with message type CM_MSG_DISPLAY
enum SRMsgKind
{
  MK_DEBUG_STRING=0x00,//!< OutputDebugString
  MK_BOX_INFO    =0x01,//!< MessageBox  ASTERISK
  MK_BOX_WARN    =0x02,//!< MessageBox  WARNING
  MK_BOX_ERR     =0x03 //!< MessageBox  HAND
};

//!Message Category: enum for function \ref SR_FuncCB with message type CM_MSG_DISPLAY
enum SRMsgCategory
{
  MC_GENERAL   =0x0000,//!< general category
  MC_DLL_FUNC  =0x0100,//!< DllMain,DllFunction messages
  MC_CAM       =0x0200,//!< Camera, USB Info messages
  MC_USB       =0x0300,//!< USB messages -> is logged to NIL because it slowed down the frame rate!
  MC_CONFIG    =0x0400,//!< configuration messages (open, read registry etc.)
  MC_FIRMWARE  =0x0500,//!< Firmware upload messages
  MC_XML       =0x0600,//!< XML messages
  MC_CAM_DLG   =0x0700,//!< A CDlgCamSettings messages
  MC_HTTP      =0x0800,//!< HTTP messages
  MC_ETH       =0x0900,//!< Ethernet messages
  MC_RESERVED  =0x0a00,//!< was MC_1394: Baumer Firewire messages
  MC_FILEIO    =0x0b00,//!< File IO messages
  MC_B_GIG_E   =0x0c00 //!< Baumer GigE messages
};

//#pragma pack(show)
#pragma pack(push,r1,4)
//#pragma pack(show)

#ifndef MATLAB_PREPROC
//!Enum and struct used in the function SR_SetParam()
//! \intDoc{structure}
typedef struct _ParamVal
{
#ifndef MATLAB_PREPROC
  //!enum to describe which SR_SetParam() function should be executed
  enum ID {
    PR_ANF=1,        //!< adaptive neighborhood filter
    PR_RESERVED0,    //!< was PR_SCATFILT: scattering filter
    PR_TEMPORAL_IIR, //!< temporal IIR filter
    PR_RESERVED1,    //!< was PR_PSEUDO_FRAME:pseudo frame acquisition/processing mode
    PR_RESERVED2,    //!< was EDGE_AMP_FLT
    PR_CONF_MAP,     //!< create a confidence map (new image in the image list)
    PR_SET_FRQ       //!< sets modulation frq without autoload (needed for calibration)
  };
  //!enum to describe what kind of data is in the structure
  enum ParamType {
    PT_UINT8,     //!< unsigned char
    PT_INT8,      //!< char
    PT_UINT16,    //!< unsinged short
    PT_INT16,     //!< short
    PT_UINT32,    //!< unsinged int
    PT_INT32,     //!< int
    PT_FLOAT,     //!< float
    PT_DOUBLE,    //!< double
    PT_WCHAR,     //!< wide char
    PT_PTR_UINT8, //!< pointer to unsigned char
    PT_PTR_WCHAR  //!< pointer to wide char
  };
  
  enum ParamType t;   //!< the paramtype of this entry
  union
  {
    unsigned char  u8;
    char           s8;
    unsigned short u16;
    short          s16;
    unsigned int   u32;
    int            s32;
    float          f32;
    double         f64;
    __wchar_t      wc16;
    unsigned char* pu8;
    __wchar_t*     pwc16;
  };
#else
  ParamVal_ParamType t;   //!< the paramtype of this entry
  double             data;
#endif
}ParamVal;//!<typedef struct _ParamVal
#pragma pack(pop,r1)
#endif
//#pragma pack(show)
//!@}

//!\ingroup libMesaSRinternal

//! \intDoc{enumeator}
enum IOmode {IO_USB_BULK=1,   //!<read/write bulk data to the USB device the address is the endpoint value (0x02 for sr3k,sr4k)
             IO_FLASH_FX2,    //!<read/write FX2 flash memory (SR3k,SR4k)
             IO_FLASH_IPM,    //!<read/write IPM flash memory (SR4k)
             IO_FLASH_CIM_ETH,//!<read/write blackfin flash memory (ETH-SR4k)
             IO_SERIAL,        //!<read/write the serial number of the camera
             IO_LAST
             };

//! \intDoc{enumeator}
enum CamType{CT_UNKNOWN=0,CT_SR2A,CT_SR2B,CT_SR3K_USB,CT_SR3K_ETH,CT_SR4K_USB,CT_SR4K_ETH,CT_SR4K_B_GIG_E,CT_ARTTS,CT_LAST};
//!@}

//!\ingroup libMesaSRdepreciated
//!this is a deprecated message callback
//!\deprecated
typedef int (MessageCB)(int level, const char* string);

