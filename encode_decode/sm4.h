/**
 * \file sm4.h
 */
#ifndef XYSSL_SM4_H
#define XYSSL_SM4_H
 
#ifdef __cplusplus
extern "C" {
#endif
 
#define SM4_ENCRYPT     1
#define SM4_DECRYPT     0
 
/**
 * \brief          SM4�����Ľṹ
 */
typedef struct
{
    int mode;                   /*!<  encrypt/decrypt  ���ܽ���  */
    unsigned long sk[32];       /*!<  SM4 subkeys     SM4�ؼ���  */
}sm4_context;
 
 
/**
 * \brief          SM4 key schedule (128-bit, encryption) SM4��Կ����(128λ������)
 *
 * \param ctx      SM4 context to be initialized        SM4������
 * \param key      16-byte secret key       16�ֽ���Կ
 */
void sm4_setkey_enc( sm4_context *ctx, unsigned char key[16] );
/**
 * \brief          SM4 key schedule (128-bit, decryption) SM4��Կ����(128λ������)
 *
 * \param ctx      SM4 context to be initialized        SM4������
 * \param key      16-byte secret key       16�ֽ���Կ
 */
void sm4_setkey_dec( sm4_context *ctx, unsigned char key[16] );
/**
 * \brief          SM4-ECB block encryption/decryption   SM4-ECB �����/����
 * \param ctx      SM4 context to be used               SM4������
 * \param mode     SM4_ENCRYPT or SM4_DECRYPT        
 * \param length   length of the input data              �������ݵĳ���
 * \param input    input block      �����
 * \param output   output block     �����
 */
void sm4_crypt_ecb( sm4_context *ctx,
				     int mode,
					 int length,
                     unsigned char *input,
                     unsigned char *output);

void sm4_crypt_cbc( sm4_context *ctx,
					 int mode,
					 int length,
					 unsigned char iv[16],
					 unsigned char *input,
					 unsigned char *output );

#ifdef __cplusplus
}
#endif
 
#endif /* sm4.h */
 
