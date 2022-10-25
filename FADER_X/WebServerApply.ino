static unsigned short defaultMessageWaitMillis = 50;
static byte defaultMotorMinSpeed = 100;
static unsigned short defaultMotorFrequency = 256;
static byte defaultMotorScaleFactor = 3;
static byte defaultStaticIP[4] = {192, 168, 1, 130};
static byte defaultSubnetIP[4] = {255, 255, 255, 0};
static byte defaultGatewayIP[4] = {192, 168, 1, 1};
static byte defaultDestinationIP[4] = {192, 168, 1, 120};

boolean applySettings(EthernetClient*client, String*b){

  Serial.println("Apply Settings");
  
  int tok = containsParam(b, "sessionToken");

  if(tok==-1){
    Serial.println("Missing token - do not save!");
    return false;

  }else{
    pauseAllFaders();
    globalNewSettingsFlag = true;
    
    char token[4];
    String tokenParam = getParameter(b, tok);
    tokenParam.toCharArray(token, 4);

//    EEPROM.write(8, token[0]);
//    EEPROM.write(9, token[1]);
//    EEPROM.write(10, token[2]);
//    EEPROM.write(11, token[3]);

    sessionToken[0] = token[0];
    sessionToken[1] = token[1];
    sessionToken[2] = token[2];
    sessionToken[3] = token[3];
    
  }

  int t0 = containsParam(b, "t0");
  int t1 = containsParam(b, "t1");
  int t2 = containsParam(b, "t2");
  int t3 = containsParam(b, "t3");
  int rot = containsParam(b, "rot");
  int mb = containsParam(b, "mb");

  int op = containsParam(b, "op");

  int n1 = containsParam(b, "n1");
  int n2 = containsParam(b, "n2");
  int n3 = containsParam(b, "n3");
  int n4 = containsParam(b, "n4");
  int n5 = containsParam(b, "n5");
  int n6 = containsParam(b, "n6");
  int n7 = containsParam(b, "n7");

  int m91 = containsParam(b, "m91");
  int m92 = containsParam(b, "m92");

  int ch1A = containsParam(b, "ch1A");
  int ch2A = containsParam(b, "ch2A");
  int ch3A = containsParam(b, "ch3A");
  int ch4A = containsParam(b, "ch4A");
  int ch5A = containsParam(b, "ch5A");
  int ch6A = containsParam(b, "ch6A");
  int ch7A = containsParam(b, "ch7A");
  int ch8A = containsParam(b, "ch8A");

  int ch1B = containsParam(b, "ch1B");
  int ch2B = containsParam(b, "ch2B");
  int ch3B = containsParam(b, "ch3B");
  int ch4B = containsParam(b, "ch4B");
  int ch5B = containsParam(b, "ch5B");
  int ch6B = containsParam(b, "ch6B");
  int ch7B = containsParam(b, "ch7B");
  int ch8B = containsParam(b, "ch8B");

  int ch1C = containsParam(b, "ch1C");
  int ch2C = containsParam(b, "ch2C");
  int ch3C = containsParam(b, "ch3C");
  int ch4C = containsParam(b, "ch4C");
  int ch5C = containsParam(b, "ch5C");
  int ch6C = containsParam(b, "ch6C");
  int ch7C = containsParam(b, "ch7C");
  int ch8C = containsParam(b, "ch8C");

  int ch1D = containsParam(b, "ch1D");
  int ch2D = containsParam(b, "ch2D");
  int ch3D = containsParam(b, "ch3D");
  int ch4D = containsParam(b, "ch4D");
  int ch5D = containsParam(b, "ch5D");
  int ch6D = containsParam(b, "ch6D");
  int ch7D = containsParam(b, "ch7D");
  int ch8D = containsParam(b, "ch8D");

  int q1 = containsParam(b, "q1");
  int q2 = containsParam(b, "q2");
  int q3 = containsParam(b, "q3");
  int q4 = containsParam(b, "q4");
  int q5 = containsParam(b, "q5");

  int e1 = containsParam(b, "e1");
  int e2 = containsParam(b, "e2");

  int x1 = containsParam(b, "x1");
  
  // Motor Settings

  if(t0){
    globalMessageWaitMillis = getParameter(b, t0).toInt();
    if(globalMessageWaitMillis>0 && globalMessageWaitMillis<=1000){
      EEPROM.put(14, globalMessageWaitMillis);
    }else{
      EEPROM.put(14, defaultMessageWaitMillis);
    }
  }
  if(t1){
    byte x = getParameter(b, t1).toInt();
    if(x>20 && x<256){
      globalMotorMinSpeed = x;
    }else{
      globalMotorMinSpeed = defaultMotorMinSpeed;
    }
    EEPROM.write(16, globalMotorMinSpeed);
  }
  if(t2){
    globalMotorSpeedScale = getParameter(b, t2).toInt();
    EEPROM.write(17, globalMotorSpeedScale);
  }
  if(t3){
    unsigned short x = getParameter(b, t3).toInt();
    if(globalMotorFrequency>0){
      globalMotorFrequency = x;
    }else{
      globalMotorFrequency = defaultMotorFrequency;
    }
    EEPROM.put(18, globalMotorFrequency);
    
  }
  if(rot){
    EEPROM.write(20, 1);
  }else{
    EEPROM.write(20, 0);
  }
  if(mb){
    byte x = getParameter(b, mb).toInt();
    if(x==1 || x==2){
      EEPROM.write(21, x);
    }
  }

  if(op){
    int globalMode = getParameter(b, op).toInt();
    EEPROM.write(24, globalMode);
    globalMode = OP_Dance;
  }

  // Network Settings

  if(n1){
    byte ethernetMode = getParameter(b, n1).toInt();
    EEPROM.write(32, ethernetMode);
  }
  if(n2){
    String ethernetStatic = getParameter(b, n2);
    int p1 = ethernetStatic.indexOf(".");
    int p2 = ethernetStatic.indexOf(".", p1+1);
    int p3 = ethernetStatic.indexOf(".", p2+1);

    if(p3>p2 && p2>p1){
      EEPROM.write(34, (byte)ethernetStatic.substring(0, p1).toInt());
      EEPROM.write(35, (byte)ethernetStatic.substring(p1+1, p2).toInt());
      EEPROM.write(36, (byte)ethernetStatic.substring(p2+1, p3).toInt());
      EEPROM.write(37, (byte)ethernetStatic.substring(p3+1).toInt());
    }else{
      EEPROM.write(34, defaultStaticIP[0]);
      EEPROM.write(35, defaultStaticIP[1]);
      EEPROM.write(36, defaultStaticIP[2]);
      EEPROM.write(37, defaultStaticIP[3]);
    }

  }
  if(n3){
    String ethernetSubnet = getParameter(b, n3);
    int p1 = ethernetSubnet.indexOf(".");
    int p2 = ethernetSubnet.indexOf(".", p1+1);
    int p3 = ethernetSubnet.indexOf(".", p2+1);

    if(p3>p2 && p2>p1){
      EEPROM.write(38, (byte)ethernetSubnet.substring(0, p1).toInt());
      EEPROM.write(39, (byte)ethernetSubnet.substring(p1+1, p2).toInt());
      EEPROM.write(40, (byte)ethernetSubnet.substring(p2+1, p3).toInt());
      EEPROM.write(41, (byte)ethernetSubnet.substring(p3+1).toInt());
    }else{
      EEPROM.write(38, defaultSubnetIP[0]);
      EEPROM.write(39, defaultSubnetIP[1]);
      EEPROM.write(40, defaultSubnetIP[2]);
      EEPROM.write(41, defaultSubnetIP[3]);
    }

  }
  if(n4){
    String ethernetGateway = getParameter(b, n4);
    int p1 = ethernetGateway.indexOf(".");
    int p2 = ethernetGateway.indexOf(".", p1+1);
    int p3 = ethernetGateway.indexOf(".", p2+1);

    if(p3>p2 && p2>p1){
      EEPROM.write(42, (byte)ethernetGateway.substring(0, p1).toInt());
      EEPROM.write(43, (byte)ethernetGateway.substring(p1+1, p2).toInt());
      EEPROM.write(44, (byte)ethernetGateway.substring(p2+1, p3).toInt());
      EEPROM.write(45, (byte)ethernetGateway.substring(p3+1).toInt());
    }else{
      EEPROM.write(42, defaultGatewayIP[0]);
      EEPROM.write(43, defaultGatewayIP[1]);
      EEPROM.write(44, defaultGatewayIP[2]);
      EEPROM.write(45, defaultGatewayIP[3]);
    }

  }
  if(n5){
    int ethernetPort = getParameter(b, n5).toInt();
    EEPROM.put(46, ethernetPort);
  }
  if(n6){
    String ethernetDest = getParameter(b, n6);
    int p1 = ethernetDest.indexOf(".");
    int p2 = ethernetDest.indexOf(".", p1+1);
    int p3 = ethernetDest.indexOf(".", p2+1);

    if(p3>p2 && p2>p1){
      EEPROM.write(48, (byte)ethernetDest.substring(0, p1).toInt());
      EEPROM.write(49, (byte)ethernetDest.substring(p1+1, p2).toInt());
      EEPROM.write(50, (byte)ethernetDest.substring(p2+1, p3).toInt());
      EEPROM.write(51, (byte)ethernetDest.substring(p3+1).toInt());
    }else{
      EEPROM.write(48, defaultDestinationIP[0]);
      EEPROM.write(49, defaultDestinationIP[1]);
      EEPROM.write(50, defaultDestinationIP[2]);
      EEPROM.write(51, defaultDestinationIP[3]);
    }

  }
  if(n7){
    int ethernetDestPort = getParameter(b, n7).toInt();
    EEPROM.put(52, ethernetDestPort);
    
  }

  // MIDI Settings

  if(m91){
    int midiListen = getParameter(b, m91).toInt();
    midi.listenChannel = midiListen;
    EEPROM.write(62, midiListen);
  }
  if(m92){
    int midiSend = getParameter(b, m92).toInt();
    midi.sendChannel = midiSend;
    EEPROM.write(63, midiSend);
  }

  // QLab Settings

  if(q1){
    int val = getParameter(b, q1).toInt();
    EEPROM.write(128, val);
  }
  if(q2){
    int val = abs(getParameter(b, q2).toInt());
    EEPROM.write(129, val);
  }
  if(q3){
    EEPROM.write(130, 1);
  }else{
    EEPROM.write(130, 0);
  }
  if(q4){
    EEPROM.write(131, 1);
  }else{
    EEPROM.write(131, 0);
  }
  if(q5){
    String passcode = getParameter(b, q5);
    EEPROM.write(132, passcode[0]);
    EEPROM.write(133, passcode[1]);
    EEPROM.write(134, passcode[2]);
    EEPROM.write(135, passcode[3]);
    EEPROM.write(136, passcode[4]);
    EEPROM.write(137, passcode[5]);
    EEPROM.write(138, passcode[6]);
    EEPROM.write(139, passcode[7]);
  }

  // EOS Settings

  if(e1){ EEPROM.write(140, getParameter(b, e1).toInt()); }
  if(e2){ EEPROM.write(141, getParameter(b, e2).toInt()); }

  // X32/M32 Settings

  if(x1){
    byte val = getParameter(b, x1).toInt();
    if(val>=1 && val<=6){
      EEPROM.write(150, getParameter(b, x1).toInt());
    }else{
      EEPROM.write(150, 1);
    }
  }

  // Channels
  
  if(ch1A){
    EEPROM.write(66, getParameter(b, ch1A).toInt());
  }
  if(ch2A){
    EEPROM.write(67, getParameter(b, ch2A).toInt());
  }
  if(ch3A){
    EEPROM.write(68, getParameter(b, ch3A).toInt());
  }
  if(ch4A){
    EEPROM.write(69, getParameter(b, ch4A).toInt());
  }
  if(ch5A){
    EEPROM.write(70, getParameter(b, ch5A).toInt());
  }
  if(ch6A){
    EEPROM.write(71, getParameter(b, ch6A).toInt());
  }
  if(ch7A){
    EEPROM.write(72, getParameter(b, ch7A).toInt());
  }
  if(ch8A){
    EEPROM.write(73, getParameter(b, ch8A).toInt());
  }


  if(ch1B){
    EEPROM.write(74, getParameter(b, ch1B).toInt());
  }
  if(ch2B){
    EEPROM.write(75, getParameter(b, ch2B).toInt());
  }
  if(ch3B){
    EEPROM.write(76, getParameter(b, ch3B).toInt());
  }
  if(ch4B){
    EEPROM.write(77, getParameter(b, ch4B).toInt());
  }
  if(ch5B){
    EEPROM.write(78, getParameter(b, ch5B).toInt());
  }
  if(ch6B){
    EEPROM.write(79, getParameter(b, ch6B).toInt());
  }
  if(ch7B){
    EEPROM.write(80, getParameter(b, ch7B).toInt());
  }
  if(ch8B){
    EEPROM.write(81, getParameter(b, ch8B).toInt());
  }


  if(ch1C){
    EEPROM.write(82, getParameter(b, ch1C).toInt());
  }
  if(ch2C){
    EEPROM.write(83, getParameter(b, ch2C).toInt());
  }
  if(ch3C){
    EEPROM.write(84, getParameter(b, ch3C).toInt());
  }
  if(ch4C){
    EEPROM.write(85, getParameter(b, ch4C).toInt());
  }
  if(ch5C){
    EEPROM.write(86, getParameter(b, ch5C).toInt());
  }
  if(ch6C){
    EEPROM.write(87, getParameter(b, ch6C).toInt());
  }
  if(ch7C){
    EEPROM.write(88, getParameter(b, ch7C).toInt());
  }
  if(ch8C){
    EEPROM.write(89, getParameter(b, ch8C).toInt());
  }


  if(ch1D){
    EEPROM.write(90, getParameter(b, ch1D).toInt());
  }
  if(ch2D){
    EEPROM.write(91, getParameter(b, ch2D).toInt());
  }
  if(ch3D){
    EEPROM.write(92, getParameter(b, ch3D).toInt());
  }
  if(ch4D){
    EEPROM.write(93, getParameter(b, ch4D).toInt());
  }
  if(ch5D){
    EEPROM.write(94, getParameter(b, ch5D).toInt());
  }
  if(ch6D){
    EEPROM.write(95, getParameter(b, ch6D).toInt());
  }
  if(ch7D){
    EEPROM.write(96, getParameter(b, ch7D).toInt());
  }
  if(ch8D){
    EEPROM.write(97, getParameter(b, ch8D).toInt());
  }
  

  if(n1 || n2 || n3 || n4 || n5 || n6){
    Ethernet.begin(net.IP_Static, net.IP_Subnet, net.IP_Gateway);
    //Udp.begin(net.IP_SelfPort);
  }

  return true;

}
