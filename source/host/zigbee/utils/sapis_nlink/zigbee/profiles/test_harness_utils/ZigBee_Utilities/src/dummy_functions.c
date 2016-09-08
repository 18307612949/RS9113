/*
 * dummy_functions.c
 *
 *  Created on: Nov 11, 2013
 *      Author: root
 */

#include "ZigBeeDataTypes.h"

void psm_readFromNVM(void);

uint8_t halCommonReadFromNvm(void *data, uint32_t offset, uint16_t length)
{
	int8 status = rsi_802154_read_from_nvm(data, offset, length)
	return ((uint8_t)status);
}

uint8_t halCommonWriteToNvm(void *data, uint32_t offset, uint16_t length)
{
	int8 status = rsi_802154_write_to_nvm(data, offset, length)
	return ((uint8_t)status);
}

void halReboot()
{

}

uint8_t psm_storeToNVM ( void )
{

	uint8_t index;
    uint8_t status;
    PSM_Info_t* pPSMInfo = stkConfig_getPSMTablePtr();

    for ( index = 0; index < g_PSM_Info_Entries_c; index++ ) {
        /*
         * Write the various tables to be stored, taking the address info from
         * the PSM info structure.
         */
        status = halCommonWriteToNvm(pPSMInfo[index].p_data_location,
                                     MAC_PIB_NVM_OFFSET + pPSMInfo[index].nvm_offset,
                                     pPSMInfo[index].data_size);


    return status;

}

uint8_t psm_clear ( void )
{
    uint8_t index;
    uint8_t status;
    PSM_Info_t* pPSMInfo = stkConfig_getPSMTablePtr();

    for ( index = 0; index < g_PSM_Info_Entries_c; index++ ) {
        /*
         * Write the various tables to be stored, taking the address info from
         * the PSM info structure.
         */
        memUtils_memSet (pPSMInfo[index].p_data_location, 0xFF, pPSMInfo[index].data_size);
        status = halCommonWriteToNvm(pPSMInfo[index].p_data_location,
                                     MAC_PIB_NVM_OFFSET + pPSMInfo[index].nvm_offset,
                                     pPSMInfo[index].data_size);

    return status;
}

uint8_t psm_readFromNVM ( void )
{

	uint8_t index;
    uint8_t status;
    PSM_Info_t* pPSMInfo = stkConfig_getPSMTablePtr();

    for ( index = 0; index < g_PSM_Info_Entries_c; index++ ) {
        /*
         * Write the various tables to be stored, taking the address info from
         * the PSM info structure.
         */
        status = halCommonReadFromNvm(pPSMInfo[index].p_data_location,
                                      MAC_PIB_NVM_OFFSET + pPSMInfo[index].nvm_offset,
                                      pPSMInfo[index].data_size);

    return status;

}
