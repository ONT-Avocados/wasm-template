
//system apis
void * calloc(int count,int length);
void * malloc(int size);
int arrayLen(void *a);
int memcpy(void * dest,void * src,int length);
int memset(void * dest,char c,int length);

//utility apis
int strcmp(char *a,char *b);
char * strconcat(char *a,char *b);
int Atoi(char * s);
long long Atoi64(char *s);
char * Itoa(int a);
char * I64toa(long long amount,int radix);
char * SHA1(char *s);
char * SHA256(char *s);

//parameter apis
int ONT_ReadInt32Param(char *args);
long long ONT_ReadInt64Param(char * args);
char * ONT_ReadStringParam(char * args);
char ** ONT_ReadStringArrayParam(char * args);
char ** ONT_ReadNestedArrayParam(char *args);
void ONT_JsonUnmashalInput(void * addr,int size,char * arg);
char * ONT_JsonMashalResult(void * val,char * types,int succeed);
char * ONT_JsonMashalParams(void * s);
char * ONT_RawMashalParams(void *s);
char * ONT_GetCallerAddress();
char * ONT_GetSelfAddress();
char * ONT_CallContract(char * address,char * contractCode,char * method,char * args);
char * ONT_MarshalNativeParams(void * s);
char * ONT_MarshalNeoParams(void * s);
void ONT_ResetParamIdx();

//Runtime apis
int ONT_Runtime_CheckWitness(char * address);
void ONT_Runtime_Notify(char ** msg);
int ONT_Runtime_CheckSig(char * pubkey,char * data,char * sig);
int ONT_Runtime_GetTime();
void ONT_Runtime_Log(char * message);
void ONT_Runtime_RaiseException(char * message);
char * ONT_Runtime_GetCurrentBlockHash();
char * ONT_Runtime_GetCodeContainer();
char * ONT_Runtime_GetExecutingAddress();
char * ONT_Runtime_GetCallingAddress();
char * ONT_Runtime_GetEntryAddress();
char * ONT_Runtime_AddressToBase58();
char * ONT_Runtime_AddressToHex();


//Attribute apis
int ONT_Attribute_GetUsage(char * data);
char * ONT_Attribute_GetData(char * data);

//Block apis
char * ONT_Block_GetCurrentHeaderHash();
int ONT_Block_GetCurrentHeaderHeight();
char * ONT_Block_GetCurrentBlockHash();
int ONT_Block_GetCurrentBlockHeight();
char * ONT_Block_GetTransactionByHash(char * hash);
int * ONT_Block_GetTransactionCountByBlkHash(char * hash);
int * ONT_Block_GetTransactionCountByBlkHeight(int height);
char ** ONT_Block_GetTransactionsByBlkHash(char * hash);
char ** ONT_Block_GetTransactionsByBlkHeight(int height);


//Blockchain apis
int ONT_BlockChain_GetHeight();
char * ONT_BlockChain_GetHeaderByHeight(int height);
char * ONT_BlockChain_GetHeaderByHash(char * hash);
char * ONT_BlockChain_GetBlockByHeight(int height);
char * ONT_BlockChain_GetBlockByHash(char * hash);
char * ONT_BlockChain_GetContract(char * address);

//header apis
char * ONT_Header_GetHash(char * data);
int ONT_Header_GetVersion(char * data);
char * ONT_Header_GetPrevHash(char * data);
char * ONT_Header_GetMerkleRoot(char  * data);
int ONT_Header_GetIndex(char * data);
int ONT_Header_GetTimestamp(char * data);
long long ONT_Header_GetConsensusData(char * data);
char * ONT_Header_GetNextConsensus(char * data);

//storage apis
void ONT_Storage_Put(char * key,char * value);
char * ONT_Storage_Get(char * key);
void ONT_Storage_Delete(char * key);

//transaction apis
char * ONT_Transaction_GetHash(char * data);
int ONT_Transaction_GetType(char * data);
char * ONT_Transaction_GetAttributes(char * data);

//for debug only
void ContractLogDebug(char * msg);
void ContractLogInfo(char * msg);
void ContractLogError(char * msg);

char ** TOKENID_LIST = {"01","02","03","04","05"};
char * NAME = "Name";
char * SYMBOL = "Symbol";
char * BALANCE = "Balance";
char * TOTAL_SUPPLY = "Totalsupply";
char * Approve = "Approve";
char * INITED = "Initialized";


char * _concatKey(char * str1, char* str2)
{
    return strconcat(strconcat(str1, "_"),str2);
}

char * _checkTokenid(char * tokenid)
{
    if (arrayLen(ONT_Storage_Get(_concatKey(NAME,tokenid))) > 0)
    {
        return "true";
    }
    return "false";
}

void notifyTransfer( char * fromAddr, char * toAddr, char * tokenid, long long amount)
{
      char ** str = (char **)malloc(sizeof(char*) * 5);
      str[0] = "transfer";
      str[1] = fromAddr;
      str[2] = toAddr;
      str[3] = tokenid;
      str[4] = I64toa(amount,10);

      ONT_Runtime_Notify(str);
}

void notifyApproval(char * owner,char * spender,char * tokenid, long long amount)
{
    char ** str = (char **)malloc(sizeof(char*) * 5);
    str[0] = "approval";
    str[1] = owner;
    str[2] = spender;
    str[3] = tokenid;
    str[4] = I64toa(amount,10);

    ONT_Runtime_Notify(str);
}


char * name(char * tokenid)
{
    return ONT_Storage_Get(_concatKey(NAME,tokenid));
}

char * symbol(char * tokenid)
{
    return ONT_Storage_Get(_concatKey(SYMBOL,tokenid));
}

char * balanceOf(char * account,char * tokenid)
{
    return ONT_Storage_Get(_concatKey(_concatKey(BALANCE,tokenid),account));
}

char * transfer(char * fromaddr, char * toaddr, char * tokenid, long long amount)
{
    if (ONT_Runtime_CheckWitness(fromaddr) == "false")
    {
        return "false";
    }
    if(amount <= 0)
    {
        return "false";
    }
    char * fromkey = _concatKey(BALANCE,fromaddr);
    long long fromBalance = Atoi64(ONT_Storage_Get(fromkey));
    if (fromBalance < amount)
    {
        return "false";
    }
    char * tokey = _concatKey(BALANCE, toaddr);
    long long toBalance = Atoi64(ONT_Storage_Get(tokey));
    toBalance += amount;
    ONT_Storage_Put(tokey,I64toa( toBalance,10));
    if (amount == fromBalance)
    {
        ONT_Storage_Delete(fromkey);
    }
    else
    {
        fromBalance -= amount;
        ONT_Storage_Put(tokey, I64toa(fromBalance,10));
    }
    
    notifyTransfer(fromaddr,toaddr,tokenid,amount);
    return "true";

}

char * approve(char * fromaddr, char * spender, char * tokenid, long long amount)
{
    if (ONT_Runtime_CheckWitness(fromaddr) == "false")
    {
        return "false";
    }
    if(amount <= 0)
    {
        return "false";
    }
    char * approvekey = _concatKey(_concatKey(_concatKey(Approve,tokenid),fromaddr),spender);
    ONT_Storage_Put(approvekey,I64toa(amount,10));
    notifyApproval(fromaddr,spender,tokenid,amount);
    return "true";
}

char * transferFrom(char * fromaddr, char * spender, char * toaddr, char * tokenid, long long amount)
{
    if (ONT_Runtime_CheckWitness(spender) == "false")
    {
        return "false";
    }

    if (amount <= 0)
    {
        return "false";
    }

    char * approvekey = _concatKey(_concatKey(_concatKey(Approve,tokenid),fromaddr),spender);
    long long allowance = Atoi64(ONT_Storage_Get(approvekey));
    if (allowance < amount)
    {
        return "false";
    }

    long long fromBalance = Atoi64(balanceOf(fromaddr,tokenid));
    if (fromBalance < amount)
    {
        return "false";
    }

    if (allowance == amount)
    {
        ONT_Storage_Delete(approvekey);
    }
    else
    {
        ONT_Storage_Put(approvekey,I64toa(allowance - amount,10));
    }
    
    char * fromkey = _concatKey( _concatKey(BALANCE,tokenid),fromaddr);

    if (fromBalance == amount)
    {
        ONT_Storage_Delete(fromkey);
    }
    else
    {
        ONT_Storage_Put(fromkey,I64toa(fromBalance - amount,10));
    }
    
    char *tokey = _concatKey(_concatKey(BALANCE,tokenid),toaddr);
    long long tobalance = balanceOf(toaddr,tokenid);
    ONT_Storage_Put(tokey,tobalance + amount);
    notifyTransfer(fromaddr,toaddr,tokenid,amount);
    return "true";
}


char * Invoke(char * method, char * args)
{
    if (strcmp(method, "name") == 0)
    {
        char * tokenid = ONT_ReadStringParam(args);
        return name(tokenid);
    }
    if (strcmp(method,"symbol") == 0)
    {
        char * tokenid = ONT_ReadStringParam(args);
        return symbol(tokenid);
    }
    if (strcmp(method,"totalSupply") == 0)
    {
        char * tokenid = ONT_ReadStringParam(args);
        return totalSupply(tokenid);
    }
    if (strcmp(method,"balanceOf")==0)
    {
        char * addr = ONT_ReadStringParam(args);
        char * tokenid = ONT_ReadStringParam(args);

        return balanceOf(addr,tokenid);
    }
    if (strcmp(method,"transfer")==0)
    {
        char * fromaddr = ONT_ReadStringParam(args);
        char * toaddr = ONT_ReadStringParam(args);
        char * tokenid = ONT_ReadStringParam(args);
        long long amount = ONT_ReadInt64Param(args);

        return transfer(fromaddr, toaddr, tokenid, amount);

    }
    if(strcmp(method,"approve") == 0)
    {
        char * fromaddr = ONT_ReadStringParam(args);
        char * toaddr = ONT_ReadStringParam(args);
        char * tokenid = ONT_ReadStringParam(args);
        long long amount = ONT_ReadInt64Param(args);
        return approve(fromaddr, toaddr, tokenid, amount);
    }
    if(strcmp(method,"transferFrom") == 0)
    {
        char * fromaddr = ONT_ReadStringParam(args);
        char * spender = ONT_ReadStringParam(args);
        char * toaddr = ONT_ReadStringParam(args);
        char * tokenid = ONT_ReadStringParam(args);
        long long amount =  ONT_ReadInt64Param(args);
        return transferFrom(fromaddr,spender,toaddr,tokenid,amount);
    }
}
