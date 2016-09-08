/**
 * @file     rsi_nwk.h
 * @version  0.1
 * @date     15 Aug 2015 
 *
 *  Copyright(C) Redpine Signals 2015
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief : This file contains networking related information
 *
 *  @section Description  This file contains networking relating information
 *
 *
 */
#ifndef RSI_NWK_H
#define RSI_NWK_H

//! common includes

/******************************************************
 * *                      Macros
 * ******************************************************/

//! flag bit to set IPV6 
#define  RSI_IPV6                                     BIT(0)   

//! flags bit to enable SSL
#define  RSI_SSL_ENABLE                               BIT(1)

//! flag bit to enable JSON associated to the webpage
#define  RSI_WEB_PAGE_JSON_ENABLE                     BIT(3)

//! ipversion v4
#define  RSI_IP_VERSION_4               4                  

//! ipversion v6
#define  RSI_IP_VERSION_6               6


/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/
typedef struct rsi_nwk_callback_s
{

  void   (*nwk_error_call_back_handler)(uint8_t command_type , uint32_t status, const uint8_t *buffer, const uint32_t length);

  //! http response notify handler pointer
  void   (*http_client_response_handler)(uint16_t status,const uint8_t *buffer, const uint16_t length, const uint32_t more_data);
  
  //! smtp mail sent response notify handler pointer
  void   (*smtp_client_mail_response_handler)(uint16_t status,const uint8_t cmd_type );
  
  //! smtp client delete response notify handler pointer
  void   (*smtp_client_delete_response_handler)(uint16_t status,const uint8_t cmd_type);
  
  //! FTP client directory list 
  void   (*ftp_directory_list_call_back_handler)(uint16_t status, uint8_t *directory_list, uint16_t length , uint8_t end_of_list);
  
  //! FTP client file read callback handler
  void   (*ftp_file_read_call_back_handler)(uint16_t status, uint8_t *file_content, uint16_t content_length, uint8_t end_of_file);
  //! Callback for data transfer complete notification
  void   (*data_transfer_complete_handler)(uint8_t sockID, uint16_t length);
  
    //! webpage request handler
  void   (*rsi_webpage_request_handler)(uint8_t type, uint8_t *url_name, uint8_t *post_content_buffer, uint32_t post_content_length, uint32_t status);

  //! Json object upadte response handler
  void   (*rsi_json_object_update_handler)(uint8_t *file_name, uint8_t *json_object, uint32_t length, uint32_t status);
  
  //! wireless firmware upgrade handler
  void   (*rsi_wireless_fw_upgrade_handler)(uint8_t *file_name, uint8_t *json_object, uint32_t length, uint32_t status);
  

}rsi_nwk_callback_t;



//! network control block
typedef struct rsi_nwk_cb_s
{
  //! network callbacks
  rsi_nwk_callback_t nwk_callbacks;

}rsi_nwk_cb_t;

//! enumerations for nwk call back types
typedef enum rsi_nwk_callback_id_e
{
  RSI_NWK_ERROR_CB                           =   0,
  RSI_WLAN_NWK_URL_REQ_CB                    =   1,
  RSI_WLAN_NWK_JSON_UPDATE_CB                =   2,
  RSI_WLAN_NWK_FW_UPGRADE_CB                 =   3
}rsi_nwk_callback_id_t;


uint16_t rsi_nwk_register_callbacks(rsi_nwk_callback_id_t callback_id, void (*callback_handler_ptr)(uint8_t command_type , uint32_t status, const uint8_t *buffer, const uint32_t length));
uint16_t rsi_wlan_nwk_register_json_update_cb(uint32_t callback_id, void (*callback_handler_ptr)(uint8_t *filename, uint8_t *json_object, uint32_t length, uint32_t status));
uint16_t rsi_wlan_nwk_register_webpage_req_cb(uint32_t callback_id, void (*callback_handler_ptr)(uint8_t type, uint8_t *url_name, uint32_t post_content_length, uint32_t status));
uint16_t rsi_wlan_nwk_register_wireless_fw_upgrade_cb(uint32_t callback_id, void (*callback_handler_ptr)(uint8_t type, uint32_t status));



/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/

//! HTTP CLIENT application includes

/******************************************************
 * *                      Macros
 * ******************************************************/

//! HTTP GET
#define RSI_HTTP_GET                 0

//! HTTP POST
#define RSI_HTTP_POST                 1

//! HTTP OFFSET
#define RSI_HTTP_OFFSET               12

//! Buffer length 
#define  RSI_HTTP_BUFFER_LEN             1200

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/

//! HTTP Get/Post request structure
typedef struct rsi_req_http_client_s
{

  //! ip version 
  uint8_t ip_version[2];

  //! https enable 
  uint8_t https_enable[2];
  
  //! port number
  uint16_t port;
  
  //! buffer
  uint8_t buffer[RSI_HTTP_BUFFER_LEN];

}rsi_req_http_client_t;

/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
int32_t rsi_http_client_async(uint8_t type, uint8_t flags, uint8_t *ip_address, uint16_t port, 
                            uint8_t *resource, uint8_t *host_name, uint8_t *extended_header, 
                            uint8_t *user_name, uint8_t *password, uint8_t *post_data, uint16_t post_data_length,
                            void(*callback)(uint16_t status, const uint8_t *buffer, const uint16_t length, uint32_t moredata));





//! DNS Application includes

/******************************************************
 * *                      Macros
 * ******************************************************/

#define MAX_URL_LEN                  90
#define MAX_DNS_REPLIES              10


/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                    Structures
*******************************************************/


//! DNS Server add request structure
typedef struct rsi_req_dns_server_add_s 
{
  //! Ip version value 
  uint8_t  ip_version[2];

  //! Dns mode to use
  uint8_t  dns_mode[2];

  //! Primary DNS addresss
  union
  {
    uint8_t  primary_dns_ipv4[4];
    uint8_t  primary_dns_ipv6[16];
  }ip_address1;

  //! Seconadary DNS address
  union
  {
    uint8_t  secondary_dns_ipv4[4];
    uint8_t  secondary_dns_ipv6[16];
  }ip_address2;
} rsi_req_dns_server_add_t;



//!  DNS Query request structure
 
typedef struct rsi_req_dns_query_s 
{
  //! Ip version value 
  uint8_t ip_version[2];

  //! URL name 
  uint8_t url_name [MAX_URL_LEN];

  //! DNS servers count
  uint8_t dns_server_number[2];

} rsi_req_dns_query_t;



//! DNS Server add response structure
typedef struct rsi_rsp_dns_server_add_s 
{
  //! Primary DNS srever address
  union
  {
    uint8_t primary_dns_ipv4_addr[4];
    uint8_t primary_dns_ipv6_addr[16];
  }ip_address1;

  //! Secondary DNS srever address
  union
  {
    uint8_t secondary_dns_ipv4_addr[4];
    uint8_t secondary_dns_ipv6_addr[16];
  }ip_address2;
} rsi_rsp_dns_server_add_t;




//! DNS Query response structure
 
typedef struct rsi_rsp_dns_query_s
{
  //! Ip version of the DNS server
  uint8_t  ip_version[2];

  //! DNS response count
  uint8_t  ip_count[2];

  //! DNS address responses
  union
  {
    uint8_t ipv4_address[4];
    uint8_t ipv6_address[16];
  }ip_address[MAX_DNS_REPLIES];
} rsi_rsp_dns_query_t;



/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/

int32_t rsi_dns_req(uint8_t ip_version, uint8_t *url_name, uint8_t *server_address, rsi_rsp_dns_query_t *dns_query_resp, uint16_t length);



 //! FTP Client feature related prototypes
/******************************************************
 * *                      Macros
 * ******************************************************/

//! Max length of FTP server login username
#define RSI_FTP_USERNAME_LENGTH 31
//! Max length of FTP server login password
#define RSI_FTP_PASSWORD_LENGTH 31

//! Max length of FTP file path
#define RSI_FTP_PATH_LENGTH 51
//! Max chunk length in a command
#define RSI_FTP_MAX_CHUNK_LENGTH 1400



/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
//! FTP client commands
typedef enum rsi_ftp_commands_e
{
  //! Creates FTP objects. This should be the first command for accessing FTP
  RSI_FTP_CREATE  =   1,

  //! Command to Connect to FTP server.
  RSI_FTP_CONNECT,                
 
  //! Command to create directory in a specified path. 
  RSI_FTP_DIRECTORY_CREATE,    

  //! Command to delete directory in a specified path
  RSI_FTP_DIRECTORY_DELETE,  

  //! Command to change working directory to a specified path.
  RSI_FTP_DIRECTORY_SET,

  //! Command to list directory contents in a specified path
  RSI_FTP_DIRECTORY_LIST,     

  //! Command to read the file
  RSI_FTP_FILE_READ,         

  //! Command to write the file
  RSI_FTP_FILE_WRITE,

  //! Command to write the content in file already opened
  RSI_FTP_FILE_WRITE_CONTENT, 
  
  //! Command to delete the specified file 
  RSI_FTP_FILE_DELETE,        

  //! Command to Rename the specified file 
  RSI_FTP_FILE_RENAME, 

  //! Disconnects from FTP server.
  RSI_FTP_DISCONNECT,        

  //! Destroys FTP objects
  RSI_FTP_DESTROY            
}rsi_ftp_commands_t;



/******************************************************
 * *                    Structures
*******************************************************/

//! FTP connect request structure
typedef struct rsi_ftp_connect_s
{
  //! Command type
  uint8_t  command_type;

  //! FTP client IP version
  uint8_t ip_version;

  union
  {
    //! IPv4 address
    uint8_t  ipv4_address[4];

    //! IPv6 address
    uint8_t  ipv6_address[16];
  } server_ip_address;

  //! FTP client username
  uint8_t username[RSI_FTP_USERNAME_LENGTH];

  //! FTP client password
  uint8_t password[RSI_FTP_PASSWORD_LENGTH];

  //! FTP server port number
  uint8_t server_port[4];

} rsi_ftp_connect_t;




//! FTP file rename request structure
typedef struct rsi_ftp_file_rename_s
{

  //! Command type
  uint8_t  command_type;

  //! Directory or file path
  uint8_t old_file_name[RSI_FTP_PATH_LENGTH];

  //! New file name
  uint8_t new_file_name[RSI_FTP_PATH_LENGTH];

} rsi_ftp_file_rename_t;




//! FTP file operations request structure
typedef struct rsi_ftp_file_ops_s
{

  //! Command type
  uint8_t  command_type;

  //! Directory or file path
  uint8_t file_name[RSI_FTP_PATH_LENGTH];


}rsi_ftp_file_ops_t;





//! FTP file write request structure
typedef struct rsi_ftp_file_write_s
{
  //! command type
  uint8_t command_type;

  //! End of file
  uint8_t end_of_file;

  //! Path of file to write
  uint8_t file_content[RSI_FTP_MAX_CHUNK_LENGTH];

} rsi_ftp_file_write_t;



//! FTP file operations response structure
typedef struct rsi_ftp_file_rsp_s
{
  //! command type
  uint8_t command_type;
  
  //! Data pending 
  uint8_t  more;

  //! data length 
  uint16_t data_length;

  //! Data content 
  uint8_t  data_content[1024];

}rsi_ftp_file_rsp_t;




/******************************************************
 * *               Function Declarations
 * ******************************************************/
 

int32_t rsi_ftp_connect(uint16_t flags, int8_t *server_ip, int8_t *username, int8_t *password, uint32_t server_port);
int32_t rsi_ftp_disconnect(void);
int32_t rsi_ftp_file_write(int8_t *file_name);
int32_t rsi_ftp_file_write_content(uint16_t flags, int8_t *file_content,int16_t content_length,uint8_t end_of_file);

int32_t rsi_ftp_file_read_aysnc(int8_t *file_name, void (*call_back_handler_ptr)(uint16_t status, uint8_t *file_content, uint16_t content_length, uint8_t end_of_file));
int32_t rsi_ftp_file_delete(int8_t *file_name);
int32_t rsi_ftp_file_rename(int8_t *old_file_name, int8_t *new_file_name);
int32_t rsi_ftp_directory_create(int8_t *directory_name);
int32_t rsi_ftp_directory_delete(int8_t *directory_name);
int32_t rsi_ftp_directory_set(int8_t *directory_path);
int32_t rsi_ftp_directory_list_async(int8_t *directory_path,void (*call_back_handler_ptr)(uint16_t status, uint8_t *directory_list, uint16_t length , uint8_t end_of_list));



//! SMTP client Application includes

/******************************************************
 * *                      Macros
 * ******************************************************/

//! SMTP feature

#define  RSI_SMTP_BUFFER_LENGTH            1024
#define  RSI_SMTP_CLIENT_CREATE               1
#define  RSI_SMTP_CLIENT_INIT                 2
#define  RSI_SMTP_CLIENT_MAIL_SEND            3
#define  RSI_SMTP_CLIENT_DEINIT               4           


/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/

//! Define for SMTP client initialization 

typedef struct rsi_smtp_client_init_s
{
  //! SMTP server ip version
  uint8_t ip_version;

  union
  {
    //! Server ipv4 address
    uint8_t  ipv4_address[4];

    //! Server ipv6 address
    uint8_t  ipv6_address[16];

  }server_ip_address;
  
  //! SMTP server authentication type
  uint8_t  auth_type;

  //! SMTP server port number
  uint8_t server_port[4];

}rsi_smtp_client_init_t;


//! Define for SMTP client mail send 
typedef struct rsi_smtp_mail_send_s 
{
  //! SMTP mail priority level
  uint8_t smtp_feature;

  //! SMTP mail body length
  uint8_t mail_body_length[2];

}rsi_smtp_mail_send_t;


//! Define SMTP client structure 
typedef struct rsi_req_smtp_client_s 
{
  //! SMTP client command type
  uint8_t command_type;

  //! SMTP client command structure
  union
  {
    rsi_smtp_client_init_t  smtp_client_init;
    rsi_smtp_mail_send_t    smtp_mail_send;
  
  } smtp_struct;

  uint8_t  smtp_buffer[RSI_SMTP_BUFFER_LENGTH];

} rsi_req_smtp_client_t;

//! SMTP response structure
typedef struct rsi_rsp_smtp_client_s
{
  //! Receive SMTP command type
  uint8_t command_type;

}rsi_rsp_smtp_client_t;




/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/

//! multicast related prototypes
/******************************************************
 * *                      Macros
 * ******************************************************/

//! Multicast join command
#define RSI_MULTICAST_JOIN              1

//! Multicast leave command
#define RSI_MULTICAST_LEAVE             0

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/

//! Multicast request structure
typedef struct rsi_req_multicast
{
  //! IP version
  uint8_t ip_version[2];

  //! command type
  uint8_t type[2];

  union
  {
    uint8_t ipv4_address[4];
    uint8_t ipv6_address[16];
  } multicast_address;

} rsi_req_multicast_t;

/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
 
 //! HTTP server Application includes

/******************************************************
 * *                      Macros
 * ******************************************************/

 
#define RSI_MAX_URL_LENGTH                      40
#define RSI_MAX_FILE_NAME_LENGTH                24
#define RSI_MAX_POST_DATA_LENGTH                512
#define RSI_MAX_WEBPAGE_SEND_SIZE               1024
#define RSI_MAX_HOST_WEBPAGE_SEND_SIZE          1400

#define RSI_JSON_MAX_CHUNK_LENGTH               1024
#define RSI_WEB_PAGE_CLEAR_ALL_FILES            1
#define RSI_WEB_PAGE_HAS_JSON                   1

/******************************************************
 * *                 Global Variables
 * ******************************************************/

/******************************************************
 * *                    Structures
*******************************************************/

typedef struct
{
   uint8_t   url_length;
   uint8_t   url_name[RSI_MAX_URL_LENGTH];
   uint8_t   request_type;
   uint8_t   post_content_length[2];
   uint8_t   post_data[RSI_MAX_POST_DATA_LENGTH];

}rsi_urlReqFrameRcv; 


//! Define Webpage load request structure
typedef struct rsi_webpage_load_s 
{ 
    //! file name of the webpage
    uint8_t   filename[RSI_MAX_FILE_NAME_LENGTH];

    //! Total length of the webpage
    uint8_t   total_len[2];

    //! Current length of the webapge
    uint8_t   current_len[2];

    //! webpage associated json object flag
    uint8_t   has_json_data;

    //! Webpage content
    uint8_t   webpage[RSI_MAX_WEBPAGE_SEND_SIZE];

} rsi_webpage_load_t;


//! Define json object load request structure
typedef struct rsi_json_object_load_s
{
    //! File name of the json object
    uint8_t   filename[RSI_MAX_FILE_NAME_LENGTH];

    //! Total length of the json object data
    uint8_t   total_length[2];

    //! Current length of the json object
    uint8_t   current_length[2];

    //! Json object data
    uint8_t   json_object[RSI_JSON_MAX_CHUNK_LENGTH];

} rsi_json_object_load_t;


//! Define webpage erase all request structure
typedef struct rsi_webpage_erase_all_s
{
    //! Flag to clear the file
    uint8_t   clear;

} rsi_webpage_erase_all_t;


//! Define webpage erase request structure
typedef struct rsi_wbpage_erase_s
{
    //! Filename of the webpage or json object data
    char    filename[RSI_MAX_FILE_NAME_LENGTH];

} rsi_wbpage_erase_t;


//! Define json object erase request structure
typedef struct rsi_json_object_erase_s
{
    //! Filename of the webpage or json object data
    char    filename[RSI_MAX_FILE_NAME_LENGTH];

} rsi_json_object_erase_t;


//! Host web page send request structure 
typedef struct rsi_webpage_send_s 
{  
    //! Total length of the webpage
    uint8_t   total_len[4];

    //! More chunks flag
    uint8_t   more_chunks;

    //! Webpage content
    uint8_t   webpage[RSI_MAX_HOST_WEBPAGE_SEND_SIZE];

} rsi_webpage_send_t;



/******************************************************
 * *               Function Declarations
 * ******************************************************/


int32_t rsi_webpage_load(uint8_t flags, uint8_t *file_name, uint8_t *webpage, uint32_t length);
int32_t rsi_json_object_create(uint8_t flags, uint8_t *file_name, uint8_t *json_object, uint32_t length);
int32_t rsi_webpage_erase(uint8_t *file_name);
int32_t rsi_json_object_delete(uint8_t *file_name);
int32_t rsi_webpage_send(uint8_t flags, uint8_t *webpage, uint32_t length);



//! MDNS-SD structure definations

/******************************************************
 * *                      Macros
 * ******************************************************/
//! MDNSD buffer size
#define MDNSD_BUFFER_SIZE           1000

//! MDNSD init
#define RSI_MDNSD_INIT               1

//! MDNSD register service
#define RSI_MDNSD_REGISTER_SERVICE   3

//! MDNSD deinit
#define RSI_MDNSD_DEINIT             6

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/

//! mdnsd init structure
typedef struct rsi_mdnsd_init_s
{
  //! ip version
  uint8_t      ip_version;

  //! time to live
  uint8_t      ttl[2];

} rsi_mdnsd_init_t;

//! mdnsd register-service structure 
typedef struct rsi_mdnsd_reg_srv_s
{
  //! port number
  uint8_t      port[2];

  //! time to live
  uint8_t      ttl[2];

  //! reset if it is last service in the list
  uint8_t      more;

} rsi_mdnsd_reg_srv_t;

//! mdnsd structure
typedef struct rsi_req_mdnsd_t 
{
  //! command type 1-MDNSD init, 3- Register service, 6- Deinit
  uint8_t    command_type;
  union 
  {
    //! mdnsd init
    rsi_mdnsd_init_t      mdnsd_init;

    //! mdnsd register
    rsi_mdnsd_reg_srv_t   mdnsd_register_service;     

  } mdnsd_struct;
   
  //! buffer
  uint8_t    buffer[MDNSD_BUFFER_SIZE];

} rsi_req_mdnsd_t;


/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
#endif
