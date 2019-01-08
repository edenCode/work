#测试服务器
openssl s_server  -key privkey.pem -cert CAcert.pem -accept 2009

#测试客户端
openssl s_client -connect localhost:20