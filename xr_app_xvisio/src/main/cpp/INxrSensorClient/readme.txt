1，初始化
	NxrMoudle moudle;
    bool ret =  moudle.open("libnxrsensor_client.so");
    if(ret){
        vsc = (INxrSensorClient *)moudle.getNxrClient();
    }
    // vsc = new NativeVSC();
    if(!ret || vsc == NULL){
        return EXIT_SUCCESS;
    }
    status = vsc->initVSC();
	
2，销毁
	vsc->uninit();
3，获取rgb相机数据
	vsc->startStreaming(StreamType type,void(*f)(StreamType type,NxrStreamData *data));
	结构体类型见，NxrSensorTypes.h
3，HID读写
    vsc->hidWrite(unsigned char *buff,int length);//写
	vsc->hieRead(unsigned char *buff,int length);//读