#���Է�����
openssl s_server  -key privkey.pem -cert CAcert.pem -accept 2009

#���Կͻ���
openssl s_client -connect localhost:20