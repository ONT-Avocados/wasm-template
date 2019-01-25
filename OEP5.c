
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

char * admin = "Ad4pjz2bqep4RhQrUAzMuZJkBC3qJ1tZuT";

char * NAME = "My Non-Fungbles Token";
char * SYMBOL = "MNFT";

char * OWNER_BALANCE_PREFIX = "Balance";
char * OWNER_OF_TOKEN_PREFIX = "OwnerOf";
char * APPROVE_PREFIX = "Approve";
char * TOKEN_ID_PREFIX = "TokenID";
char * TOKEN_INDEX_PREFIX = "Index";
char * TOTAL_SUPPLY = "TotalSupply";
char * INITED = "Initialized";

//concat to string with "_"
char * _concatkey(char * str1, char * str2)
{
    return strconcat(strconcat(str1,"_"),str2);
}

void notifyTransfer( char * fromAddr, char * toAddr, char * tokenID)
{
      char ** str = (char **)malloc(sizeof(char*) * 4);
      str[0] = "transfer";
      str[1] = fromAddr;
      str[2] = toAddr;
      str[3] = tokenID;

      ONT_Runtime_Notify(str);
}

void notifyApproval(char * fromAddr,char * toAddr, char * tokenID)
{
    char ** str = (char **)malloc(sizeof(char*) * 4);
    str[0] = "approval";
    str[1] = fromAddr;
    str[2] = toAddr;
    str[3] = tokenID;

    ONT_Runtime_Notify(str);
}

char * name()
{
    return NAME;
}
char * symbol()
{
    return SYMBOL;
}
char * balanceOf(char * address)
{
    char * key = _concatkey(OWNER_BALANCE_PREFIX,address);
    return ONT_Storage_Get(key);
}
char * ownerOf(char * tokenID)
{
    char * key = _concatkey(TOKEN_ID_PREFIX,tokenID);
    return ONT_Storage_Get(key);
}

char * transfer(char * fromaddr, char * toaddr, char * tokenID)
{
    if (ONT_Runtime_CheckWitness(fromaddr) != 1)
    {
        return "false";
    }
    char * owner = ownerOf(tokenID);
    if (strcmp(owner,fromaddr) != 0 )
    {
        return "false";
    }
    //1. change the ownership
    ONT_Storage_Put(_concatkey(OWNER_OF_TOKEN_PREFIX,tokenID),toaddr);
    //2. add toaddr balance amount
    long long tobalance = Atoi64(balanceOf(toaddr));
    ONT_Storage_Put(_concatkey(OWNER_BALANCE_PREFIX,toaddr),I64toa(tobalance + 1,10));
    //3. decrease from addr balance amount
    long long fromBalance = Atoi64(balanceOf(fromaddr));
    if (fromBalance - 1 > 0)
    {
        ONT_Storage_Put(_concatkey(OWNER_BALANCE_PREFIX,fromaddr),I64toa(fromBalance - 1,10));
    }
    else
    {
        ONT_Storage_Delete(_concatkey(OWNER_BALANCE_PREFIX,fromaddr));
    }
    
    notifyTransfer(fromaddr,toaddr,tokenID);
    return "true";

}

char * approve(char * toaddr, char * tokenID)
{
    char * owner = ownerOf(tokenID);
    if(ONT_Runtime_CheckWitness(owner) != 1)
    {
        return "false";
    }
    ONT_Storage_Put(_concatkey(APPROVE_PREFIX,tokenID),toaddr);
    notifyApproval(owner,toaddr,tokenID);
    return "false";
}

char * takeOwnership(char * toaddr, char * tokenID)
{
    char * key = _concatkey(APPROVE_PREFIX,tokenID);
    char * approval = ONT_Storage_Get(key);

    if(ONT_Runtime_CheckWitness(approval) != 1)
    {
        return "false";
    }
    char * owner = ownerOf(tokenID);
    if (arrayLen(owner) == 0)
    {
        return "false";
    }
    //change the token owner to toaddr
    char * ownerkey = _concatkey(OWNER_OF_TOKEN_PREFIX,tokenID);
    ONT_Storage_Put(ownerkey,toaddr);

    //change the balance
    //2. add toaddr balance amount
    long long tobalance = Atoi64(balanceOf(toaddr));
    ONT_Storage_Put(_concatkey(OWNER_BALANCE_PREFIX,toaddr),I64toa(tobalance + 1,10));
    //3. decrease from addr balance amount
    long long fromBalance = Atoi64(balanceOf(owner));
    if (fromBalance - 1 > 0)
    {
        ONT_Storage_Put(_concatkey(OWNER_BALANCE_PREFIX,owner),I64toa(fromBalance - 1,10));
    }
    else
    {
        ONT_Storage_Delete(_concatkey(OWNER_BALANCE_PREFIX,owner));
    }

    notifyTransfer(owner,toaddr,tokenID);
    return "true";

}


char * Invoke(char * method, char * args)
{
    if(strcmp(method,"name") == 0)
    {
        return name();
    }
    if(strcmp(method,"symbol") == 0)
    {
        return symbol();
    }
    if(strcmp(method, "balanceOf") == 0)
    {
        char * address = ONT_ReadStringParam(args); 
        return balanceOf(address);
    }
    if(strcmp(method,"ownerOf") == 0)
    {
        char * tokenId = ONT_ReadStringParam(args);
        return ownerOf(tokenId);
    }
    if(strcmp(method,"transfer") == 0)
    {
        char * fromaddr = ONT_ReadStringParam(args);
        char * toaddr = ONT_ReadStringParam(args);
        char * tokenID = ONT_ReadStringParam(args);

        return transfer(fromaddr,toaddr,tokenID);
    }
    if(strcmp(method,"approve") == 0)
    {
        char * toaddr = ONT_ReadStringParam(args);
        char * tokenID = ONT_ReadStringParam(args);

        return approve(toaddr,tokenID);
    }
    if(strcmp(method,"takeOwnership") == 0)
    {
        char * toaddr = ONT_ReadStringParam(args);
        char * tokenID = ONT_ReadStringParam(args);

        return takeOwnership(toaddr,tokenID);
    }


    return "false";
}
