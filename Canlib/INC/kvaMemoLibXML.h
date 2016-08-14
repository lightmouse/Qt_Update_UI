/*
 **                   Copyright 2005 by KVASER AB, SWEDEN
 **                        WWW: http://www.kvaser.com
 **
 ** This software is furnished under a license and may be used and copied
 ** only in accordance with the terms of such license.
 **
 ** Description:
 **   Library for converting XML settings to a binary param.lif for Kvaser
 **   Memorator 2nd Generation
 **
 **  The binary settings used by Kvaser Eagle are extensive and an API that
 **  covers all possibilities would be very complex. A better approach is to use
 **  XML to describe the settings and parse them into a binary settings file
 **  with an external library.
 **
 **  The XML conversion results in a binary settings file, param.lif, that can
 **  be downloaded to a Kvaser Eagle with the KvmLib API call kvmWriteConfig()
 **
 ** ---------------------------------------------------------------------------
 */

#ifndef KVAMEMOLIBXML_H
#define KVAMEMOLIBXML_H


#include <windows.h>
#include <winioctl.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \name XML_ERROR_MESSAGE_LENGTH
 * \ingroup Conversion
 * \anchor XML_ERROR_MESSAGE_LENGTH
 *
 * Maximum length of the xml error message string.
 *
 * @{
 */
#define XML_ERROR_MESSAGE_LENGTH 2048  ///< Maximum length of the xml error message string.
/** @} */

/**
 * \name KvaXmlStatus
 * \ingroup Conversion
 * \anchor KvaXmlStatusERR_XXX
 *
 * Generally, a return code greater than or equal to zero means success. A
 * value less than zero means failure.
 *
 * @{
 */
typedef enum {
  KvaXmlStatusOK                    =  0,  ///< OK
  KvaXmlStatusFail                  = -1,  ///< Generic error
  KvaXmlStatusERR_ATTR_NOT_FOUND    = -3,  ///< Failed to find an attribute in a node
  KvaXmlStatusERR_ATTR_VALUE        = -4,  ///< The attribute value is not correct, e.g. whitespace after a number.
  KvaXmlStatusERR_ELEM_NOT_FOUND    = -5,  ///< Could not find a required element
  KvaXmlStatusERR_VALUE_RANGE       = -6,  ///< The value is outside the allowed range
  KvaXmlStatusERR_VALUE_UNIQUE      = -7,  ///< The value is not unique; usually idx attributes
  KvaXmlStatusERR_VALUE_CONSECUTIVE = -8,  ///< The values are not consecutive; usually idx attributes
  KvaXmlStatusERR_EXPRESSION        = -9,  ///< The trigger expression could not be parsed
  KvaXmlStatusERR_XML_PARSER        = -10, ///< The XML settings contain syntax errors.
  KvaXmlStatusERR_DTD_VALIDATION    = -11, ///< The XML settings do not follow the DTD.
  KvaXmlStatusERR_SCRIPT_ERROR      = -12, ///< t-script related errors, e.g. file not found.
  KvaXmlStatusERR_INTERNAL          = -20, ///< Internal errors, e.g. null pointers.
} KvaXmlStatus;
/** @} */

/**
 * \name KvaXmlValidationStatus
 * \ingroup Validation
 * \anchor KvaXmlValidationStatusERR_XXX
 *
 * Generally, a return code greater than or equal to zero means success. A
 * value less than zero means failure.
 *
 * @{
 */
typedef enum {
  KvaXmlValidationStatusOK                         =  0,  ///< OK.
  KvaXmlValidationStatusFail                       = -1,  ///< Generic error
  KvaXmlValidationStatusERR_ABORT                  = -2,  ///< Too many errors, validation aborted
  KvaXmlValidationStatusERR_SILENT_TRANSMIT        = -3,  ///< Transmit lists used in silent mode
  KvaXmlValidationStatusERR_UNDEFINED_TRIGGER      = -4,  ///< An undefined trigger is used in an expression
  KvaXmlValidationStatusERR_MULTIPLE_EXT_TRIGGER   = -5,  ///< There are more than one external trigger defined
  KvaXmlValidationStatusERR_MULTIPLE_START_TRIGGER = -6,  ///< There are more than one start up trigger defined
  KvaXmlValidationStatusERR_DISK_FULL_STARTS_LOG   = -7,  ///< A trigger on disk full starts the logging
  KvaXmlValidationStatusERR_NUM_OUT_OF_RANGE       = -8,  ///< A numerical value is out of range
  KvaXmlValidationStatusERR_SCRIPT_NOT_FOUND       = -9,  ///< A t-script file could not be opened
  KvaXmlValidationStatusERR_SCRIPT_TOO_LARGE       = -10, ///< A t-script is too large for the configuration
  KvaXmlValidationStatusERR_SCRIPT_TOO_MANY        = -11, ///< Too many active t-scripts for selected device
  KvaXmlValidationStatusERR_SCRIPT_CONFLICT        = -12, ///< More than one active script is set as 'primary'
  KvaXmlValidationStatusERR_ELEMENT_COUNT          = -13, ///< Too many or too few elements of this type

  KvaXmlValidationStatusWARN_ABORT               = -100,  ///< Too many warnings, validation aborted
  KvaXmlValidationStatusWARN_NO_ACTIVE_LOG       = -101,  ///< No active logging detected
  KvaXmlValidationStatusWARN_DISK_FULL_AND_FIFO  = -102,  ///< A trigger on disk full used with FIFO mode
  KvaXmlValidationStatusWARN_IGNORED_ELEMENT     = -103,  ///< This XML element was ignored
} KvaXmlValidationStatus;
/** @} */

/**
 * \ingroup Initialization
 *
 * \source_cs       <b>KvaXmlStatus kvaXmlInitialize(void);</b>
 * \source_end
 *
 * This function must be called before any other functions are used.  It will
 * initialize the kvamemolibxml library.
 *
 * \return \ref KvaXmlStatusOK (zero) if success.
 * \return \ref KvaXmlStatusERR_XXX (negative) if failure.
 */
KvaXmlStatus WINAPI kvaXmlInitialize (void);

/**
 * \ingroup Initialization
 *
 * Get the last error message (if any) from the conversion in human readable
 * format. Use the macro \ref XML_ERROR_MESSAGE_LENGTH to allocate the buffer
 * buf.
 *
 * \param[out] buf           Buffer to receive error text.
 * \param[in]  len           Buffer size in bytes.
 * \param[in]  err           The error code to convert.
 *
 * \return \ref KvaXmlStatusOK (zero) if success.
 * \return \ref KvaXmlStatusERR_XXX (negative) if failure.
 */
KvaXmlStatus WINAPI kvaXmlGetLastError (char *buf, unsigned int len, KvaXmlStatus *err);

/**
 * \ingroup Conversion
 *
 * Convert the XML settings from buffer xmlbuf with length xmllen. The
 * resulting param.lif is written to the buffer outbuf and has length outlen.
 * Use the macro PARAMLIF_SIZE to allocate the ouput buffer to ensure that
 * it is sufficiently large. The version of the XML settings is returned in
 * version (Upper 16 bits: major, lower 16 bits: minor).
 *
 * \param[in]   xmlbuf           Buffer containing the XML settings.
 * \param[in]   xmllen           Size of the XML buffer in bytes.
 * \param[out]  outbuf           Buffer to receive the param.lif settings.
 * \param[out]  outlen           Size of the param.lif buffer in bytes.
 * \param[out]  version          XML version.
 *
 * \return \ref KvaXmlStatusOK (zero) if success.
 * \return \ref KvaXmlStatusERR_XXX (negative) if failure.
 */
KvaXmlStatus WINAPI kvaXmlToBuffer (const char *xmlbuf, unsigned int xmllen, char *outbuf, unsigned int *outlen, long *version);

/**
 * \ingroup Conversion
 *
 * Convert the XML settings from infile and write the binary settings to
 * outfile.
 *
 * \param[in]   infile            Path and name of the file containing the XML settings.
 * \param[out]  outfile           Path and name of the file to receive the param.lif settings.
 *
 * \return \ref KvaXmlStatusOK (zero) if success.
 * \return \ref KvaXmlStatusERR_XXX (negative) if failure.
 */
KvaXmlStatus WINAPI kvaXmlToFile (const char *infile, const char *outfile);

/**
 * \ingroup Conversion
 *
 * Convert the binary settings from parfile and write the XML settings to
 * xmlfile.
 *
 * \param[in]   parfile           Path and name of the file containing the param.lif settings.
 * \param[out]  xmlfile           Path and name of the file to receive the XML settings.
 *
 * \return \ref KvaXmlStatusOK (zero) if success.
 * \return \ref KvaXmlStatusERR_XXX (negative) if failure.
 */
KvaXmlStatus WINAPI kvaFileToXml(const char * parfile, const char * xmlfile);

/**
 * \ingroup Conversion
 *
 * Enable detailed information about the XML conversion on standard out. This
 * can be very useful when the error that causes the failure is masked by
 * subsequent errors.
 *
 * \param[in]   on            Enable debug output if non-zero.
 *
 * \return \ref KvaXmlStatusOK (zero) if success.
 * \return \ref KvaXmlStatusERR_XXX (negative) if failure.
 */
KvaXmlStatus WINAPI kvaXmlDebugOutput (int on);

/**
 * \ingroup Conversion
 *
 * Convert a buffer containing param.lif with size inlen to a new XML settings
 * buffer xmlbuf with length xmllen. The version of the XML settings is
 * returned in version (Upper 16 bits: major, lower 16 bits: minor). Scripts
 * from the param.lif will be written to the directory specified in scriptpath.
 *
 * \param[in]   inbuf            Buffer containing the param.lif settings.
 * \param[in]   inlen            Size of param.lif buffer in bytes.
 * \param[out]  xmlbuf           Buffer to receive the XML settings.
 * \param[out]  xmllen           Size of the XML buffer in bytes.
 * \param[out]  version          XML version.
 * \param[in]   scriptpath       Path to destination of scripts.
 *
 * \return \ref KvaXmlStatusOK (zero) if success.
 * \return \ref KvaXmlStatusERR_XXX (negative) if failure.
 */
KvaXmlStatus WINAPI kvaBufferToXml (const char *inbuf, unsigned int inlen, char *xmlbuf, unsigned int *xmllen, long *version, const char * scriptpath);

/**
 * \ingroup Validation
 *
 * Validate a buffer with XML settings
 *
 * \param[in]   xmlbuf           Buffer containing the XML settings.
 * \param[in]   xmllen           Size of the XML buffer in bytes.
 *
 * \return \ref KvaXmlStatusOK (zero) if success.
 * \return \ref KvaXmlStatusERR_XXX (negative) if failure.
 */
KvaXmlStatus WINAPI kvaXmlValidate (const char *xmlbuf, unsigned int xmllen);

/**
 * \ingroup Validation
 *
 * Get the number of validation statuses (if any). Call after kvaXmlValidate()
 *
 * \param[out]   countWarn          Number of XML valditation errors.
 * \param[out]   countErr           Number of XML validation warnings.
 *
 * \return \ref KvaXmlStatusOK (zero) if success.
 * \return \ref KvaXmlStatusERR_XXX (negative) if failure.
 */
KvaXmlStatus WINAPI kvaXmlGetValidationStatusCount (int *countErr, int *countWarn);

/**
 * \ingroup Validation
 *
 * Get the validation errors (if any). Call after kvaXmlValidate() until
 * KvaXmlValidationStatusOK
 *
 * \param[out]   status             Valdiation status code.
 * \param[out]   buf                Buffer containing the validation error message.
 * \param[out]   len                Size of the validation message buffer in bytes.
 *
 * \return \ref KvaXmlStatusOK (zero) if success.
 * \return \ref KvaXmlStatusERR_XXX (negative) if failure.
 */
KvaXmlStatus WINAPI kvaXmlGetValidationError (KvaXmlValidationStatus *status, char *buf, unsigned int len);

/**
 * \ingroup Validation
 *
 * Get the validation warnings (if any). Call after kvaXmlValidate() until
 * KvaXmlValidationStatusOK
 *
 * \param[out]   status             Valdiation status code.
 * \param[out]   buf                Buffer containing the validation warning message.
 * \param[out]   len                Size of the validation message buffer in bytes.
 *
 * \return \ref KvaXmlStatusOK (zero) if success.
 * \return \ref KvaXmlStatusERR_XXX (negative) if failure.
 */
KvaXmlStatus WINAPI kvaXmlGetValidationWarning (KvaXmlValidationStatus *status, char *buf, unsigned int len);

/**
 * \ingroup Conversion
 *
 * Get a human readable description of error with supplied error code.
 *
 * \param[in]   status     \ref KvaXmlStatus error code.
 * \param[out]  buf        Buffer to receive error message.
 * \param[in]   len        Buffer size in bytes.
 *
 * \return \ref KvaXmlStatusOK (zero) if success.
 * \return \ref KvaXmlStatusERR_XXX (negative) if failure.
 */
KvaXmlStatus WINAPI kvaXmlGetErrorText (KvaXmlStatus status, char *buf, unsigned int len);

/**
 * \ingroup Validation
 *
 * Get a human readable description of validation error with supplied error
 * code.
 *
 * \param[in]   status     \ref KvaXmlValidationStatus error code.
 * \param[out]  buf        Buffer to receive error message.
 * \param[in]   len        Buffer size in bytes.
 *
 * \return \ref KvaXmlStatusOK (zero) if success.
 * \return \ref KvaXmlStatusERR_XXX (negative) if failure.
 */
KvaXmlStatus WINAPI kvaXmlGetValidationText (KvaXmlValidationStatus status, char *buf, unsigned int len);

/**
 * \ingroup Validation
 *
 * \source_cs       <b>KvaXmlStatus kvaXmlGetVersion(void);</b>
 * \source_end
 *
 * Return the version of the kvaMemoLibXML DLL.  The most significant byte is
 * the major version number and the least significant byte is the
 * minor version number.
 *
 * \return  Version of the kvaMemoLibXML DLL.
 */
unsigned short WINAPI kvaXmlGetVersion (void);


// Functions and definitions provided by kv_parser, used for going back and
// forth between infix and postfix notation.

/**
 * \ingroup Parsing tools
 * \anchor Token
 * \anchor Tokens
 *
 * \brief Token used when parsing postfix expressions (deprecated):
 */
typedef struct tag_token {
  int               type;       // T_xxx
  char              *name;      // Name for identifiers
  struct tag_token  *left;      // Left part of expr if this is an op
  struct tag_token  *right;     // Right dito
  int               start_pos;  // Token's start pos
  int               end_pos;    // Token's end pos
  struct tag_token  *next;      // For later memory deallocation
  int               errCode;    // ERR_xxx if this is a T_ERROR
} Token;

/**
 * \ingroup Parsing tools
 * \anchor KvParseHandle
 *
 * \brief Handle used when parsing postfix expressions (deprecated):
 */
typedef struct {
  Token *next;
} KvParseHandle;

/**
 * \ingroup Parsing tools
 *
 * \source_cs       <b>KvParseHandle kvaToolsParseCreate(void);</b>
 * \source_end
 *
 * Create a parser, the start of a linked list of \ref Tokens (deprecated).
 *
 * \return  \ref KvParseHandle of new parser.
 */
KvParseHandle* WINAPI kvaToolsParseCreate(void);

/**
 * \ingroup Parsing tools
 *
 * Destroy a parser with handle h and any linked \ref Tokens (deprecated).
 *
 * \param[in]   h           \ref KvParseHandle to parser to be destroyed.
 */
void WINAPI kvaToolsParseDestroy(KvParseHandle *h);

/**
 * \ingroup Parsing tools
 *
 * Get a human readable description of errors that occur when parsing a
 * postfix expression (deprecated).
 *
 * \param[in]   errCode     Error code from parser.
 * \param[out]  s           Buffer to receive error message.
 * \param[in]   bufsiz      Buffer size in bytes.
 */
void WINAPI kvaToolsExprGetErrorString(int errCode, char *s, size_t bufsiz);

/**
 * \ingroup Parsing tools
 *
 * Parse postifix expression expr and return a Token tree representation
 * (deprecated).
 *
 * \param[in]   h           \ref KvParseHandle to parser.
 * \param[out]  expr        String representation of postfix expression.
 * \param[in]   t           Pointer to \ref Token tree.
 *
 * \return                  0.
 */
int  WINAPI kvaToolsParseExpr(KvParseHandle *h, char* expr, Token **t);

/**
 * \ingroup Parsing tools
 *
 * Do nothing (deprecated).
 *
 * \param[in]   h           \ref KvParseHandle to parser.
 * \param[in]   t           Pointer to \ref Token t.
 *
 * \return                  0.
 */
int  WINAPI kvaToolsFreeExpr(KvParseHandle *h, Token *t);

/**
 * \ingroup Parsing tools
 *
 * Dump \ref Token tree contents if debug is enabled, otherwise do nothing
 * (deprecated).
 *
 * \param[in]   h           \ref KvParseHandle to parser.
 * \param[in]   t           Pointer to \ref Token t.
 *
 * \return                  0.
 */
int  WINAPI kvaToolsDumpExpr(KvParseHandle *h, Token *t);

/**
 * \ingroup Parsing tools
 *
 * Returns True if Token tree representation of trigger expression has errors
 * (deprecated).
 *
 * \param[in]   h           \ref KvParseHandle to parser.
 * \param[in]   t           Pointer to \ref Token t.
 *
 * \return                  TRUE if \ref Token tree contains any error tokens
 * \return                  FALSE if \ref Token tree contains no error tokens
 */
int  WINAPI kvaToolsExprHasErrors(KvParseHandle *h, Token *t);

/**
 * \ingroup Parsing tools
 *
 * Returns errorcode of first error found in Token tree (deprecated).
 *
 * \param[in]   h           \ref KvParseHandle to parser.
 * \param[in]   t           Pointer to \ref Token t.
 * \param[out]  errCode     Error code of first found error token.
 * \param[out]  pos         Position of first found error token.
 *
 * \return                  TRUE if \ref Token tree contains any error tokens
 * \return                  FALSE if \ref Token tree contains no error tokens
 */
int  WINAPI kvaToolsExprGetError(KvParseHandle *h, Token *t, int *errCode, int *pos);

#ifdef __cplusplus
}
#endif

#endif //KVAMEMOLIBXML_H
