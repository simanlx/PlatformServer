#include "StdAfx.h"
#include "ServiceUnits.h"
#include "DataBaseEngineSink.h"

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDataBaseEngineSink::CDataBaseEngineSink()
{
	//���ñ���
	m_pGameParameter=NULL;
	m_pInitParameter=NULL;
	m_pDataBaseParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//�������
	m_pIDataBaseEngine=NULL;
	m_pIGameServiceManager=NULL;
	m_pIDataBaseEngineEvent=NULL;
	m_pIGameDataBaseEngineSink=NULL;
	m_pIDBCorrespondManager=NULL;

	//��������
	ZeroMemory(&m_LogonFailure,sizeof(m_LogonFailure));
	ZeroMemory(&m_LogonSuccess,sizeof(m_LogonSuccess));

	return;
}

//��������
CDataBaseEngineSink::~CDataBaseEngineSink()
{
	//�ͷŶ���
	SafeRelease(m_pIGameDataBaseEngineSink);

	return;
}

//�ӿڲ�ѯ
VOID * CDataBaseEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE(IGameDataBaseEngine,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//��ȡ����
VOID * CDataBaseEngineSink::GetDataBase(REFGUID Guid, DWORD dwQueryVer)
{
	//Ч��״̬
	ASSERT(m_GameDBModule.GetInterface()!=NULL);
	if (m_GameDBModule.GetInterface()==NULL) return NULL;

	//��ѯ����
	IDataBase * pIDataBase=m_GameDBModule.GetInterface();
	VOID * pIQueryObject=pIDataBase->QueryInterface(Guid,dwQueryVer);

	return pIQueryObject;
}

//��ȡ����
VOID * CDataBaseEngineSink::GetDataBaseEngine(REFGUID Guid, DWORD dwQueryVer)
{
	//Ч��״̬
	ASSERT(m_pIDataBaseEngine!=NULL);
	if (m_pIDataBaseEngine==NULL) return NULL;

	//��ѯ����
	VOID * pIQueryObject=m_pIDataBaseEngine->QueryInterface(Guid,dwQueryVer);

	return pIQueryObject;
}

//Ͷ�ݽ��
bool CDataBaseEngineSink::PostGameDataBaseResult(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return true;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{
	//��ѯ����
	ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IDataBaseEngine)!=NULL);
	m_pIDataBaseEngine=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IDataBaseEngine);

	//��������
	if ((m_GameDBModule.GetInterface()==NULL)&&(m_GameDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}
	
	//��������
	if ((m_TreasureDBModule.GetInterface()==NULL)&&(m_TreasureDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if ((m_PlatformDBModule.GetInterface()==NULL)&&(m_PlatformDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//������Ϸ
	try
	{
		//������Ϣ
		tagDataBaseParameter * pTreasureDBParameter=&m_pInitParameter->m_TreasureDBParameter;
		tagDataBaseParameter * pPlatformDBParameter=&m_pInitParameter->m_PlatformDBParameter;

		//��������
		m_GameDBModule->SetConnectionInfo(m_pDataBaseParameter->szDataBaseAddr,m_pDataBaseParameter->wDataBasePort,
			m_pDataBaseParameter->szDataBaseName,m_pDataBaseParameter->szDataBaseUser,m_pDataBaseParameter->szDataBasePass);

		//��������
		m_TreasureDBModule->SetConnectionInfo(pTreasureDBParameter->szDataBaseAddr,pTreasureDBParameter->wDataBasePort,
			pTreasureDBParameter->szDataBaseName,pTreasureDBParameter->szDataBaseUser,pTreasureDBParameter->szDataBasePass);

		//��������
		m_PlatformDBModule->SetConnectionInfo(pPlatformDBParameter->szDataBaseAddr,pPlatformDBParameter->wDataBasePort,
			pPlatformDBParameter->szDataBaseName,pPlatformDBParameter->szDataBaseUser,pPlatformDBParameter->szDataBasePass);

		//��������
		m_GameDBModule->OpenConnection();
		m_GameDBAide.SetDataBase(m_GameDBModule.GetInterface());

		//��������
		m_TreasureDBModule->OpenConnection();
		m_TreasureDBAide.SetDataBase(m_TreasureDBModule.GetInterface());

		//��������
		m_PlatformDBModule->OpenConnection();
		m_PlatformDBAide.SetDataBase(m_PlatformDBModule.GetInterface());

		//���ݹ���
		ASSERT(m_pIGameServiceManager!=NULL);
		m_pIGameDataBaseEngineSink=(IGameDataBaseEngineSink *)m_pIGameServiceManager->CreateGameDataBaseEngineSink(IID_IGameDataBaseEngineSink,VER_IGameDataBaseEngineSink);

		//���ö���
		if ((m_pIGameDataBaseEngineSink!=NULL)&&(m_pIGameDataBaseEngineSink->InitializeSink(QUERY_ME_INTERFACE(IUnknownEx))==false))
		{
			//�������
			ASSERT(FALSE);

			//�����Ϣ
			CTraceService::TraceString(TEXT("��Ϸ���ݿ���չ���������������ʧ��"),TraceLevel_Exception);

			return false;
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//ֹͣ�¼�
bool CDataBaseEngineSink::OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx)
{
	//��������
	DWORD dwUserID=0;
	DBR_GR_UnlockAndroidUser UnlockAndroidUser;
	UnlockAndroidUser.wServerID=m_pGameServiceOption->wServerID;
	UnlockAndroidUser.wBatchID=0;
	OnRequestUnlockAndroidUser(0,&UnlockAndroidUser,sizeof(UnlockAndroidUser),dwUserID);

	//���ñ���
	m_pInitParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//�������
	m_pIGameServiceManager=NULL;
	m_pIDataBaseEngineEvent=NULL;

	//���ö���
	m_GameDBAide.SetDataBase(NULL);

	//�ͷŶ���
	SafeRelease(m_pIGameDataBaseEngineSink);

	//�ر�����
	if (m_GameDBModule.GetInterface()!=NULL)
	{
		m_GameDBModule->CloseConnection();
		m_GameDBModule.CloseInstance();
	}

	//�ر�����
	if (m_TreasureDBModule.GetInterface()!=NULL)
	{
		m_TreasureDBModule->CloseConnection();
		m_TreasureDBModule.CloseInstance();
	}

	//�ر�����
	if (m_PlatformDBModule.GetInterface()!=NULL)
	{
		m_PlatformDBModule->CloseConnection();
		m_PlatformDBModule.CloseInstance();
	}

	return true;
}

//ʱ���¼�
bool CDataBaseEngineSink::OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	return false;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	return false;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	bool bSucceed = false;
	DWORD dwUserID = 0L;

	//������
	switch (wRequestID)
	{
	case DBR_GR_LOGON_USERID:			//I D ��¼
		{
			bSucceed = OnRequestLogonUserID(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOGON_MOBILE:			//�ֻ���¼
		{
			bSucceed = OnRequestLogonMobile(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOGON_ACCOUNTS:			//�ʺŵ�¼
		{
			bSucceed = OnRequestLogonAccounts(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_WRITE_GAME_SCORE:		//��Ϸд��
		{
			bSucceed = OnRequestWriteGameScore(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LEAVE_GAME_SERVER:		//�뿪����
		{
			bSucceed = OnRequestLeaveGameServer(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_GAME_SCORE_RECORD:		//��Ϸ��¼
		{
			bSucceed = OnRequestGameScoreRecord(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_PARAMETER:			//���ز���
		{
			bSucceed = OnRequestLoadParameter(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_GAME_COLUMN:		//�����б�
		{
			bSucceed = OnRequestLoadGameColumn(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_ANDROID_PARAMETER:	//��������
		{
			bSucceed = OnRequestLoadAndroidParameter(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_ANDROID_USER:		//���ػ���
		{
			bSucceed = OnRequestLoadAndroidUser(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_GAME_PROPERTY:		//���ص���
		{
			bSucceed = OnRequestLoadGameProperty(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_GROWLEVEL_CONFIG:	//�ȼ�����
		{
			bSucceed = OnRequestLoadGrowLevelConfig(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_USER_ENABLE_INSURE:		//��ͨ����
		{
			bSucceed = OnRequestUserEnableInsure(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_USER_SAVE_SCORE:		//������Ϸ��
		{
			bSucceed = OnRequestUserSaveScore(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_USER_TAKE_SCORE:		//��ȡ��Ϸ��
		{
			bSucceed = OnRequestUserTakeScore(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_USER_TRANSFER_SCORE:	//ת����Ϸ��
		{
			bSucceed = OnRequestUserTransferScore(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_QUERY_INSURE_INFO:		//��ѯ����
		{
			bSucceed = OnRequestQueryInsureInfo(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_QUERY_TRANSFER_USER_INFO:		//��ѯ�û�
		{
			bSucceed = OnRequestQueryTransferUserInfo(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_PROPERTY_REQUEST:		//��������
		{
			bSucceed = OnRequestPropertyRequest(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_WRITE_USER_GAME_DATA:	//�û���Ϸ����
		{
			bSucceed = OnRequestWriteUserGameData(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MANAGE_USER_RIGHT:		//�û�Ȩ��
		{
			bSucceed = OnRequestManageUserRight(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_SYSTEM_MESSAGE:   //ϵͳ��Ϣ
		{
			bSucceed = OnRequestLoadSystemMessage(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_SENSITIVE_WORDS://�������д�
		{
			return OnRequestLoadSensitiveWords(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_UNLOCK_ANDROID_USER://����������
		{
			bSucceed = OnRequestUnlockAndroidUser(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MATCH_SIGNUP:			//��������
		{
			bSucceed = OnRequestMatchSignup(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MATCH_UNSIGNUP:			//�˳�����
		{
			bSucceed = OnRequestMatchUnSignup(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MATCH_START:			//������ʼ
		{
			bSucceed = OnRequestMatchStart(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MATCH_ELIMINATE:		//������̭
		{
			bSucceed = OnRequestMatchEliminate(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MATCH_OVER:				//��������
		{
			bSucceed = OnRequestMatchOver(dwContextID,pData,wDataSize,dwUserID);
		}
		break;	
	case DBR_GR_MATCH_REWARD:			//��������
		{
			bSucceed = OnRequestMatchReward(dwContextID,pData,wDataSize,dwUserID);
		}
		break;	
	case DBR_GR_TASK_LOAD_LIST:			//��������
		{
			bSucceed = OnRequestLoadTaskList(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_TASK_QUERY_INFO:		//��ѯ����
		{
			bSucceed = OnRequestTaskQueryInfo(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_TASK_TAKE:				//��ȡ����
		{
			bSucceed = OnRequestTaskTake(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_TASK_REWARD:			//��ȡ����
		{
			bSucceed = OnRequestTaskReward(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_MEMBER_PARAMETER:	//��Ա����
		{
			bSucceed = OnRequestLoadMemberParameter(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_PURCHASE_MEMBER:		//�����Ա
		{
			bSucceed = OnRequestPurchaseMember(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_EXCHANGE_SCORE_INGOT:	//�һ���Ϸ��
		{
			bSucceed = OnRequestExchangeScoreByIngot(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_EXCHANGE_SCORE_BEANS:	//�һ���Ϸ��
		{
			bSucceed = OnRequestExchangeScoreByBeans(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_GROWLEVEL_QUERY_IFNO:	//��ѯ�ȼ�
		{
			bSucceed = OnRequestQueryGrowLevelParameter(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	}

	//Э��֪ͨ
	if(m_pIDBCorrespondManager) m_pIDBCorrespondManager->OnPostRequestComplete(dwUserID, bSucceed);

	return bSucceed;
}

//I D ��¼
bool CDataBaseEngineSink::OnRequestLogonUserID(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID,BOOL bMatch)
{
	//ִ�в�ѯ
	DBR_GR_LogonUserID * pLogonUserID=(DBR_GR_LogonUserID *)pData;
	dwUserID = pLogonUserID->dwUserID;

	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_LogonUserID));
	if (wDataSize!=sizeof(DBR_GR_LogonUserID)) return false;

	try
	{
		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonUserID->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pLogonUserID->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@strPassword"),pLogonUserID->szPassword);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_GameDBAide.AddParameter(TEXT("@strMachineID"),pLogonUserID->szMachineID);
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);

		//��������
		if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
		{
			m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pLogonUserID->dwMatchID);
			m_GameDBAide.AddParameter(TEXT("@dwMatchNO"),pLogonUserID->dwMatchNO);	
		}

		//�������
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),m_LogonFailure.szDescribeString,sizeof(m_LogonFailure.szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_EfficacyUserID"),true);

		//�û���Ϣ
		lstrcpyn(m_LogonSuccess.szPassword,pLogonUserID->szPassword,CountArray(m_LogonSuccess.szPassword));
		lstrcpyn(m_LogonSuccess.szMachineID,pLogonUserID->szMachineID,CountArray(m_LogonSuccess.szMachineID));
	
		//�������
		CDBVarValue DBVarValue;
		m_GameDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"),false);

		return false;
	}

	return true;
}

//I D ��¼
bool CDataBaseEngineSink::OnRequestLogonMobile(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//ִ�в�ѯ
	DBR_GR_LogonMobile * pLogonMobile=(DBR_GR_LogonMobile *)pData;
	dwUserID = pLogonMobile->dwUserID;

	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_LogonMobile));
		if (wDataSize!=sizeof(DBR_GR_LogonMobile)) return false;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonMobile->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pLogonMobile->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@strPassword"),pLogonMobile->szPassword);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_GameDBAide.AddParameter(TEXT("@strMachineID"),pLogonMobile->szMachineID);
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);

		//��������
		if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
		{
			m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pLogonMobile->dwMatchID);
			m_GameDBAide.AddParameter(TEXT("@dwMatchNO"),pLogonMobile->dwMatchNO);	
		}

		//�������
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),m_LogonFailure.szDescribeString,sizeof(m_LogonFailure.szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_EfficacyMobile"),true);

		//�û���Ϣ
		lstrcpyn(m_LogonSuccess.szPassword,pLogonMobile->szPassword,CountArray(m_LogonSuccess.szPassword));
		lstrcpyn(m_LogonSuccess.szMachineID,pLogonMobile->szMachineID,CountArray(m_LogonSuccess.szMachineID));
		m_LogonSuccess.cbDeviceType=pLogonMobile->cbDeviceType;
		m_LogonSuccess.wBehaviorFlags=pLogonMobile->wBehaviorFlags;
		m_LogonSuccess.wPageTableCount=pLogonMobile->wPageTableCount;
	
		//�������
		CDBVarValue DBVarValue;
		m_GameDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),true,pLogonMobile->cbDeviceType,pLogonMobile->wBehaviorFlags,pLogonMobile->wPageTableCount);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"),true);

		return false;
	}

	return true;
}

//�ʺŵ�¼
bool CDataBaseEngineSink::OnRequestLogonAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_LogonAccounts));
		if (wDataSize!=sizeof(DBR_GR_LogonAccounts)) return false;

		//������
		DBR_GR_LogonAccounts * pLogonAccounts=(DBR_GR_LogonAccounts *)pData;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonAccounts->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@strAccounts"),pLogonAccounts->szAccounts);
		m_GameDBAide.AddParameter(TEXT("@strPassword"),pLogonAccounts->szPassword);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_GameDBAide.AddParameter(TEXT("@strMachineID"),pLogonAccounts->szMachineID);
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),m_LogonFailure.szDescribeString,sizeof(m_LogonFailure.szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_EfficacyAccounts"),true);

		//�û���Ϣ
		lstrcpyn(m_LogonSuccess.szPassword,pLogonAccounts->szPassword,CountArray(m_LogonSuccess.szPassword));
		lstrcpyn(m_LogonSuccess.szMachineID,pLogonAccounts->szMachineID,CountArray(m_LogonSuccess.szMachineID));

		//�������
		CDBVarValue DBVarValue;
		m_GameDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		OnLogonDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"),false);

		return false;
	}

	return true;
}

//��Ϸд��
bool CDataBaseEngineSink::OnRequestWriteGameScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//������
	DBR_GR_WriteGameScore * pWriteGameScore=(DBR_GR_WriteGameScore *)pData;
	dwUserID=pWriteGameScore->dwUserID;

	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_WriteGameScore));
		if (wDataSize!=sizeof(DBR_GR_WriteGameScore)) return false;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pWriteGameScore->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_GameDBAide.ResetParameter();

		//�û���Ϣ
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pWriteGameScore->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@dwDBQuestID"),pWriteGameScore->dwDBQuestID);
		m_GameDBAide.AddParameter(TEXT("@dwInoutIndex"),pWriteGameScore->dwInoutIndex);

		//����ɼ�
		m_GameDBAide.AddParameter(TEXT("@lScore"),pWriteGameScore->VariationInfo.lScore);
		m_GameDBAide.AddParameter(TEXT("@lGrade"),pWriteGameScore->VariationInfo.lGrade);
		m_GameDBAide.AddParameter(TEXT("@lInsure"),pWriteGameScore->VariationInfo.lInsure);
		m_GameDBAide.AddParameter(TEXT("@lRevenue"),pWriteGameScore->VariationInfo.lRevenue);
		m_GameDBAide.AddParameter(TEXT("@lWinCount"),pWriteGameScore->VariationInfo.dwWinCount);
		m_GameDBAide.AddParameter(TEXT("@lLostCount"),pWriteGameScore->VariationInfo.dwLostCount);
		m_GameDBAide.AddParameter(TEXT("@lDrawCount"),pWriteGameScore->VariationInfo.dwDrawCount);
		m_GameDBAide.AddParameter(TEXT("@lFleeCount"),pWriteGameScore->VariationInfo.dwFleeCount);
		m_GameDBAide.AddParameter(TEXT("@lUserMedal"),pWriteGameScore->VariationInfo.lIngot);
		m_GameDBAide.AddParameter(TEXT("@lExperience"),pWriteGameScore->VariationInfo.dwExperience);
		m_GameDBAide.AddParameter(TEXT("@lLoveLiness"),pWriteGameScore->VariationInfo.lLoveLiness);
		m_GameDBAide.AddParameter(TEXT("@dwPlayTimeCount"),pWriteGameScore->VariationInfo.dwPlayTimeCount);

		//�������
		m_GameDBAide.AddParameter(TEXT("@cbTaskForward"),pWriteGameScore->bTaskForward);

		//��������
		if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
		{
			//������Ϣ
			m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pWriteGameScore->dwMatchID);
			m_GameDBAide.AddParameter(TEXT("@dwMatchNo"),pWriteGameScore->dwMatchNO);
		}

		//������Ϣ
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);		

		//ִ�в�ѯ
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_WriteGameScore"),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//�뿪����
bool CDataBaseEngineSink::OnRequestLeaveGameServer(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//������
	DBR_GR_LeaveGameServer * pLeaveGameServer=(DBR_GR_LeaveGameServer *)pData;
	dwUserID=pLeaveGameServer->dwUserID;

	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_LeaveGameServer));
		if (wDataSize!=sizeof(DBR_GR_LeaveGameServer)) return false;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLeaveGameServer->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_GameDBAide.ResetParameter();

		//�û���Ϣ
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pLeaveGameServer->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@dwOnLineTimeCount"),pLeaveGameServer->dwOnLineTimeCount);

		//ϵͳ��Ϣ
		m_GameDBAide.AddParameter(TEXT("@dwInoutIndex"),pLeaveGameServer->dwInoutIndex);
		m_GameDBAide.AddParameter(TEXT("@dwLeaveReason"),pLeaveGameServer->dwLeaveReason);

		//��¼�ɼ�
		m_GameDBAide.AddParameter(TEXT("@lRecordScore"),pLeaveGameServer->RecordInfo.lScore);
		m_GameDBAide.AddParameter(TEXT("@lRecordGrade"),pLeaveGameServer->RecordInfo.lGrade);
		m_GameDBAide.AddParameter(TEXT("@lRecordInsure"),pLeaveGameServer->RecordInfo.lInsure);
		m_GameDBAide.AddParameter(TEXT("@lRecordRevenue"),pLeaveGameServer->RecordInfo.lRevenue);
		m_GameDBAide.AddParameter(TEXT("@lRecordWinCount"),pLeaveGameServer->RecordInfo.dwWinCount);
		m_GameDBAide.AddParameter(TEXT("@lRecordLostCount"),pLeaveGameServer->RecordInfo.dwLostCount);
		m_GameDBAide.AddParameter(TEXT("@lRecordDrawCount"),pLeaveGameServer->RecordInfo.dwDrawCount);
		m_GameDBAide.AddParameter(TEXT("@lRecordFleeCount"),pLeaveGameServer->RecordInfo.dwFleeCount);
		m_GameDBAide.AddParameter(TEXT("@lRecordUserMedal"),pLeaveGameServer->RecordInfo.lIngot);
		m_GameDBAide.AddParameter(TEXT("@lRecordExperience"),pLeaveGameServer->RecordInfo.dwExperience);
		m_GameDBAide.AddParameter(TEXT("@lRecordLoveLiness"),pLeaveGameServer->RecordInfo.lLoveLiness);
		m_GameDBAide.AddParameter(TEXT("@dwRecordPlayTimeCount"),pLeaveGameServer->RecordInfo.dwPlayTimeCount);

		//����ɼ�
		m_GameDBAide.AddParameter(TEXT("@lVariationScore"),pLeaveGameServer->VariationInfo.lScore);
		m_GameDBAide.AddParameter(TEXT("@lVariationGrade"),pLeaveGameServer->VariationInfo.lGrade);
		m_GameDBAide.AddParameter(TEXT("@lVariationInsure"),pLeaveGameServer->VariationInfo.lInsure);
		m_GameDBAide.AddParameter(TEXT("@lVariationRevenue"),pLeaveGameServer->VariationInfo.lRevenue);
		m_GameDBAide.AddParameter(TEXT("@lVariationWinCount"),pLeaveGameServer->VariationInfo.dwWinCount);
		m_GameDBAide.AddParameter(TEXT("@lVariationLostCount"),pLeaveGameServer->VariationInfo.dwLostCount);
		m_GameDBAide.AddParameter(TEXT("@lVariationDrawCount"),pLeaveGameServer->VariationInfo.dwDrawCount);
		m_GameDBAide.AddParameter(TEXT("@lVariationFleeCount"),pLeaveGameServer->VariationInfo.dwFleeCount);
		m_GameDBAide.AddParameter(TEXT("@lVariationUserMedal"),pLeaveGameServer->VariationInfo.lIngot);
		m_GameDBAide.AddParameter(TEXT("@lVariationExperience"),pLeaveGameServer->VariationInfo.dwExperience);
		m_GameDBAide.AddParameter(TEXT("@lVariationLoveLiness"),pLeaveGameServer->VariationInfo.lLoveLiness);
		m_GameDBAide.AddParameter(TEXT("@dwVariationPlayTimeCount"),pLeaveGameServer->VariationInfo.dwPlayTimeCount);

		//��������
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_GameDBAide.AddParameter(TEXT("@strMachineID"),pLeaveGameServer->szMachineID);

		//ִ�в�ѯ
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_LeaveGameServer"),false);
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//��Ϸ��¼
bool CDataBaseEngineSink::OnRequestGameScoreRecord(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//��������
		DBR_GR_GameScoreRecord * pGameScoreRecord=(DBR_GR_GameScoreRecord *)pData;
		dwUserID=INVALID_DWORD;

		//Ч�����
		ASSERT(wDataSize<=sizeof(DBR_GR_GameScoreRecord));
		ASSERT(wDataSize>=(sizeof(DBR_GR_GameScoreRecord)-sizeof(pGameScoreRecord->GameScoreRecord)));
		ASSERT(wDataSize==(sizeof(DBR_GR_GameScoreRecord)-sizeof(pGameScoreRecord->GameScoreRecord)+pGameScoreRecord->wRecordCount*sizeof(pGameScoreRecord->GameScoreRecord[0])));

		//Ч�����
		if (wDataSize>sizeof(DBR_GR_GameScoreRecord)) return false;
		if (wDataSize<(sizeof(DBR_GR_GameScoreRecord)-sizeof(pGameScoreRecord->GameScoreRecord))) return false;
		if (wDataSize!=(sizeof(DBR_GR_GameScoreRecord)-sizeof(pGameScoreRecord->GameScoreRecord)+pGameScoreRecord->wRecordCount*sizeof(pGameScoreRecord->GameScoreRecord[0]))) return false;

		//������Ϣ
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);

		//������Ϣ
		m_GameDBAide.AddParameter(TEXT("@wTableID"),pGameScoreRecord->wTableID);
		m_GameDBAide.AddParameter(TEXT("@wUserCount"),pGameScoreRecord->wUserCount);
		m_GameDBAide.AddParameter(TEXT("@wAndroidCount"),pGameScoreRecord->wAndroidCount);

		//˰�����
		m_GameDBAide.AddParameter(TEXT("@lWasteCount"),pGameScoreRecord->lWasteCount);
		m_GameDBAide.AddParameter(TEXT("@lRevenueCount"),pGameScoreRecord->lRevenueCount);

		//ͳ����Ϣ
		m_GameDBAide.AddParameter(TEXT("@dwUserMemal"),pGameScoreRecord->dwUserMemal);
		m_GameDBAide.AddParameter(TEXT("@dwPlayTimeCount"),pGameScoreRecord->dwPlayTimeCount);

		//ʱ����Ϣ
		m_GameDBAide.AddParameter(TEXT("@SystemTimeStart"),pGameScoreRecord->SystemTimeStart);
		m_GameDBAide.AddParameter(TEXT("@SystemTimeConclude"),pGameScoreRecord->SystemTimeConclude);

		//ִ�в�ѯ
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_RecordDrawInfo"),true);

		//д���¼
		if (lResultCode==DB_SUCCESS)
		{
			//��ȡ��ʶ
			DWORD dwDrawID=m_GameDBAide.GetValue_DWORD(TEXT("DrawID"));

			//д���¼
			for (WORD i=0;i<pGameScoreRecord->wRecordCount;i++)
			{
				//���ò���
				m_GameDBAide.ResetParameter();
				
				//������Ϣ
				m_GameDBAide.AddParameter(TEXT("@dwDrawID"),dwDrawID);
				m_GameDBAide.AddParameter(TEXT("@dwUserID"),pGameScoreRecord->GameScoreRecord[i].dwUserID);
				m_GameDBAide.AddParameter(TEXT("@wChairID"),pGameScoreRecord->GameScoreRecord[i].wChairID);

				//�û���Ϣ
				m_GameDBAide.AddParameter(TEXT("@dwDBQuestID"),pGameScoreRecord->GameScoreRecord[i].dwDBQuestID);
				m_GameDBAide.AddParameter(TEXT("@dwInoutIndex"),pGameScoreRecord->GameScoreRecord[i].dwInoutIndex);

				//�ɼ���Ϣ
				m_GameDBAide.AddParameter(TEXT("@lScore"),pGameScoreRecord->GameScoreRecord[i].lScore);
				m_GameDBAide.AddParameter(TEXT("@lGrade"),pGameScoreRecord->GameScoreRecord[i].lGrade);
				m_GameDBAide.AddParameter(TEXT("@lRevenue"),pGameScoreRecord->GameScoreRecord[i].lRevenue);
				m_GameDBAide.AddParameter(TEXT("@dwUserMedal"),pGameScoreRecord->GameScoreRecord[i].dwUserMemal);
				m_GameDBAide.AddParameter(TEXT("@dwPlayTimeCount"),pGameScoreRecord->GameScoreRecord[i].dwPlayTimeCount);

				//ִ�в�ѯ
				m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_RecordDrawScore"),false);
			}
		}
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		
		return false;
	}

	return true;
}

//���ز���
bool CDataBaseEngineSink::OnRequestLoadParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//��������
		DBO_GR_GameParameter GameParameter;
		ZeroMemory(&GameParameter,sizeof(GameParameter));

		//ִ�в�ѯ
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);

		//ִ�в�ѯ
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_LoadParameter"),true);

		//��ȡ��Ϣ
		if (lResultCode==DB_SUCCESS)
		{
			//������Ϣ
			GameParameter.dwMedalRate=m_GameDBAide.GetValue_DWORD(TEXT("MedalRate"));
			GameParameter.dwRevenueRate=m_GameDBAide.GetValue_DWORD(TEXT("RevenueRate"));
			GameParameter.dwExchangeRate=m_GameDBAide.GetValue_DWORD(TEXT("ExchangeRate"));
			GameParameter.dwPresentExchangeRate=m_GameDBAide.GetValue_DWORD(TEXT("PresentExchangeRate"));
			GameParameter.dwRateGold=m_GameDBAide.GetValue_DWORD(TEXT("RateGold"));

			if (m_pGameServiceOption->wServerType==GAME_GENRE_EDUCATE)
			{
				GameParameter.lEducateGrantScore=m_GameDBAide.GetValue_LONGLONG(TEXT("EducateGrantScore"));
			}

			//���齱��
			GameParameter.dwWinExperience=m_GameDBAide.GetValue_DWORD(TEXT("WinExperience"));

			//�汾��Ϣ
			GameParameter.dwClientVersion=m_GameDBAide.GetValue_DWORD(TEXT("ClientVersion"));
			GameParameter.dwServerVersion=m_GameDBAide.GetValue_DWORD(TEXT("ServerVersion"));
		
			//������Ϣ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_PARAMETER,dwContextID,&GameParameter,sizeof(GameParameter));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//�����б�
bool CDataBaseEngineSink::OnRequestLoadGameColumn(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//��������
		DBO_GR_GameColumnInfo GameColumnInfo;
		ZeroMemory(&GameColumnInfo,sizeof(GameColumnInfo));

		//ִ�в�ѯ
		m_GameDBAide.ResetParameter();
		GameColumnInfo.lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_GameColumnItem"),true);

		//��ȡ��Ϣ
		for (WORD i=0;i<CountArray(GameColumnInfo.ColumnItemInfo);i++)
		{
			//�����ж�
			if (m_GameDBModule->IsRecordsetEnd()==true) break;

			//���Ч��
			ASSERT(GameColumnInfo.cbColumnCount<CountArray(GameColumnInfo.ColumnItemInfo));
			if (GameColumnInfo.cbColumnCount>=CountArray(GameColumnInfo.ColumnItemInfo)) break;

			//��ȡ����
			GameColumnInfo.cbColumnCount++;
			GameColumnInfo.ColumnItemInfo[i].cbColumnWidth=m_GameDBAide.GetValue_BYTE(TEXT("ColumnWidth"));
			GameColumnInfo.ColumnItemInfo[i].cbDataDescribe=m_GameDBAide.GetValue_BYTE(TEXT("DataDescribe"));
			m_GameDBAide.GetValue_String(TEXT("ColumnName"),GameColumnInfo.ColumnItemInfo[i].szColumnName,CountArray(GameColumnInfo.ColumnItemInfo[i].szColumnName));

			//�ƶ���¼
			m_GameDBModule->MoveToNext();
		}

		//������Ϣ
		WORD wHeadSize=sizeof(GameColumnInfo)-sizeof(GameColumnInfo.ColumnItemInfo);
		WORD wPacketSize=wHeadSize+GameColumnInfo.cbColumnCount*sizeof(GameColumnInfo.ColumnItemInfo[0]);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_COLUMN_INFO,dwContextID,&GameColumnInfo,wPacketSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		// --Debug
		CTraceService::TraceString( L"OnRequestLoadGameColumn", TraceLevel_Exception);

		//��������
		DBO_GR_GameColumnInfo GameColumnInfo;
		ZeroMemory(&GameColumnInfo,sizeof(GameColumnInfo));

		//�������
		GameColumnInfo.cbColumnCount=0L;
		GameColumnInfo.lResultCode=pIException->GetExceptionResult();

		//������Ϣ
		WORD wPacketSize=sizeof(GameColumnInfo)-sizeof(GameColumnInfo.ColumnItemInfo);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_COLUMN_INFO,dwContextID,&GameColumnInfo,wPacketSize);

		return false;
	}

	return true;
}

//���ػ���
bool CDataBaseEngineSink::OnRequestLoadAndroidUser(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//����У��
	ASSERT(sizeof(DBR_GR_LoadAndroidUser)==wDataSize);
	if(sizeof(DBR_GR_LoadAndroidUser)!=wDataSize) return false;

	try
	{		
		//��ȡ����
		DBR_GR_LoadAndroidUser * pLoadAndroidUser = (DBR_GR_LoadAndroidUser *)pData;
        if(pLoadAndroidUser==NULL) return false;

		//��������
		DBO_GR_GameAndroidInfo GameAndroidInfo;
		ZeroMemory(&GameAndroidInfo,sizeof(GameAndroidInfo));

		//��������
		GameAndroidInfo.dwBatchID=pLoadAndroidUser->dwBatchID;

		//�û��ʻ�
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_TreasureDBAide.AddParameter(TEXT("@dwBatchID"),pLoadAndroidUser->dwBatchID);
		m_TreasureDBAide.AddParameter(TEXT("@dwAndroidCount"),pLoadAndroidUser->dwAndroidCount);

		//ִ�в�ѯ
		GameAndroidInfo.lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_LoadAndroidUser"),true);

		//��ȡ��Ϣ
		for (WORD i=0;i<CountArray(GameAndroidInfo.AndroidAccountsInfo);i++)
		{
			//�����ж�
			if (m_TreasureDBModule->IsRecordsetEnd()==true) break;

			//���Ч��
			ASSERT(GameAndroidInfo.wAndroidCount<CountArray(GameAndroidInfo.AndroidAccountsInfo));
			if (GameAndroidInfo.wAndroidCount>=CountArray(GameAndroidInfo.AndroidAccountsInfo)) break;

			//��ȡ����
			GameAndroidInfo.wAndroidCount++;
			GameAndroidInfo.AndroidAccountsInfo[i].dwUserID=m_TreasureDBAide.GetValue_DWORD(TEXT("UserID"));
			m_TreasureDBAide.GetValue_String(TEXT("LogonPass"),GameAndroidInfo.AndroidAccountsInfo[i].szPassword,CountArray(GameAndroidInfo.AndroidAccountsInfo[i].szPassword));

			//�ƶ���¼
			m_TreasureDBModule->MoveToNext();
		}

		//������Ϣ
		WORD wHeadSize=sizeof(GameAndroidInfo)-sizeof(GameAndroidInfo.AndroidAccountsInfo);
		WORD wDataSize=GameAndroidInfo.wAndroidCount*sizeof(GameAndroidInfo.AndroidAccountsInfo[0]);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_ANDROID_INFO,dwContextID,&GameAndroidInfo,wHeadSize+wDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//��������
		DBO_GR_GameAndroidInfo GameAndroidInfo;
		ZeroMemory(&GameAndroidInfo,sizeof(GameAndroidInfo));

		//�������
		GameAndroidInfo.wAndroidCount=0L;
		GameAndroidInfo.lResultCode=DB_ERROR;

		//������Ϣ
		WORD wHeadSize=sizeof(GameAndroidInfo)-sizeof(GameAndroidInfo.AndroidAccountsInfo);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_ANDROID_INFO,dwContextID,&GameAndroidInfo,wHeadSize);
	}

	return false;
}

//���ػ���
bool CDataBaseEngineSink::OnRequestLoadAndroidParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//��������
		DBO_GR_GameAndroidParameter GameAndroidParameter;
		ZeroMemory(&GameAndroidParameter,sizeof(GameAndroidParameter));

		//�û��ʻ�
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceAttrib->wKindID);
		m_TreasureDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);

		//ִ�в�ѯ
		GameAndroidParameter.lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_LoadAndroidConfigure"),true);

		//��ȡ��Ϣ
		for (WORD i=0;i<CountArray(GameAndroidParameter.AndroidParameter);i++)
		{
			//�����ж�
			if (m_TreasureDBModule->IsRecordsetEnd()==true) break;

			//���Ч��
			ASSERT(GameAndroidParameter.wParameterCount<CountArray(GameAndroidParameter.AndroidParameter));
			if (GameAndroidParameter.wParameterCount>=CountArray(GameAndroidParameter.AndroidParameter)) break;

			//��ȡ����
			GameAndroidParameter.wParameterCount++;
			GameAndroidParameter.AndroidParameter[i].dwBatchID=m_TreasureDBAide.GetValue_DWORD(TEXT("BatchID"));
			GameAndroidParameter.AndroidParameter[i].dwServiceMode=m_TreasureDBAide.GetValue_DWORD(TEXT("ServiceMode"));
			GameAndroidParameter.AndroidParameter[i].dwAndroidCount=m_TreasureDBAide.GetValue_DWORD(TEXT("AndroidCount"));
			GameAndroidParameter.AndroidParameter[i].dwEnterTime=m_TreasureDBAide.GetValue_DWORD(TEXT("EnterTime"));
			GameAndroidParameter.AndroidParameter[i].dwLeaveTime=m_TreasureDBAide.GetValue_DWORD(TEXT("LeaveTime"));
			GameAndroidParameter.AndroidParameter[i].lTakeMinScore=m_TreasureDBAide.GetValue_LONGLONG(TEXT("TakeMinScore"));
			GameAndroidParameter.AndroidParameter[i].lTakeMaxScore=m_TreasureDBAide.GetValue_LONGLONG(TEXT("TakeMaxScore"));
			GameAndroidParameter.AndroidParameter[i].dwEnterMinInterval=m_TreasureDBAide.GetValue_DWORD(TEXT("EnterMinInterval"));
			GameAndroidParameter.AndroidParameter[i].dwEnterMaxInterval=m_TreasureDBAide.GetValue_DWORD(TEXT("EnterMaxInterval"));
			GameAndroidParameter.AndroidParameter[i].dwLeaveMinInterval=m_TreasureDBAide.GetValue_DWORD(TEXT("LeaveMinInterval"));
			GameAndroidParameter.AndroidParameter[i].dwLeaveMaxInterval=m_TreasureDBAide.GetValue_DWORD(TEXT("LeaveMaxInterval"));
			GameAndroidParameter.AndroidParameter[i].dwSwitchMinInnings=m_TreasureDBAide.GetValue_DWORD(TEXT("SwitchMinInnings"));
			GameAndroidParameter.AndroidParameter[i].dwSwitchMaxInnings=m_TreasureDBAide.GetValue_DWORD(TEXT("SwitchMaxInnings"));

			//�ƶ���¼
			m_TreasureDBModule->MoveToNext();
		}

		//������Ϣ
		WORD wHeadSize=sizeof(GameAndroidParameter)-sizeof(GameAndroidParameter.AndroidParameter);
		WORD wDataSize=GameAndroidParameter.wParameterCount*sizeof(GameAndroidParameter.AndroidParameter[0]);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_ANDROID_PARAMETER,dwContextID,&GameAndroidParameter,wHeadSize+wDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//��������
		DBO_GR_GameAndroidParameter GameAndroidParameter;
		ZeroMemory(&GameAndroidParameter,sizeof(GameAndroidParameter));

		//�������
		GameAndroidParameter.wParameterCount=0L;
		GameAndroidParameter.lResultCode=DB_ERROR;

		//������Ϣ
		WORD wHeadSize=sizeof(GameAndroidParameter)-sizeof(GameAndroidParameter.AndroidParameter);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_ANDROID_PARAMETER,dwContextID,&GameAndroidParameter,wHeadSize);
	}

	return false;
}

//���ص���
bool CDataBaseEngineSink::OnRequestLoadGameProperty(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//��������
		DBO_GR_GamePropertyInfo GamePropertyInfo;
		ZeroMemory(&GamePropertyInfo,sizeof(GamePropertyInfo));

		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);

		//ִ�в�ѯ
		GamePropertyInfo.lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_LoadGameProperty"),true);

		//��ȡ��Ϣ
		for (WORD i=0;i<CountArray(GamePropertyInfo.PropertyInfo);i++)
		{
			//�����ж�
			if (m_GameDBModule->IsRecordsetEnd()==true) break;

			//���Ч��
			ASSERT(GamePropertyInfo.cbPropertyCount<CountArray(GamePropertyInfo.PropertyInfo));
			if (GamePropertyInfo.cbPropertyCount>=CountArray(GamePropertyInfo.PropertyInfo)) break;

			//��ȡ����
			GamePropertyInfo.cbPropertyCount++;
			GamePropertyInfo.PropertyInfo[i].wIndex=m_GameDBAide.GetValue_WORD(TEXT("ID"));
			GamePropertyInfo.PropertyInfo[i].wDiscount=m_GameDBAide.GetValue_WORD(TEXT("Discount"));
			GamePropertyInfo.PropertyInfo[i].wIssueArea=m_GameDBAide.GetValue_WORD(TEXT("IssueArea"));
			GamePropertyInfo.PropertyInfo[i].dPropertyCash=m_GameDBAide.GetValue_DOUBLE(TEXT("Cash"));
			GamePropertyInfo.PropertyInfo[i].lPropertyGold=m_GameDBAide.GetValue_LONGLONG(TEXT("Gold"));
			GamePropertyInfo.PropertyInfo[i].lSendLoveLiness=m_GameDBAide.GetValue_LONGLONG(TEXT("SendLoveLiness"));
			GamePropertyInfo.PropertyInfo[i].lRecvLoveLiness=m_GameDBAide.GetValue_LONGLONG(TEXT("RecvLoveLiness"));

			//�ƶ���¼
			m_GameDBModule->MoveToNext();
		}

		//������Ϣ
		WORD wHeadSize=sizeof(GamePropertyInfo)-sizeof(GamePropertyInfo.PropertyInfo);
		WORD wDataSize=GamePropertyInfo.cbPropertyCount*sizeof(GamePropertyInfo.PropertyInfo[0]);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_PROPERTY_INFO,dwContextID,&GamePropertyInfo,wHeadSize+wDataSize);
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		
		//��������
		DBO_GR_GamePropertyInfo GamePropertyInfo;
		ZeroMemory(&GamePropertyInfo,sizeof(GamePropertyInfo));

		//�������
		GamePropertyInfo.cbPropertyCount=0L;
		GamePropertyInfo.lResultCode=DB_ERROR;

		//������Ϣ
		WORD wHeadSize=sizeof(GamePropertyInfo)-sizeof(GamePropertyInfo.PropertyInfo);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_PROPERTY_INFO,dwContextID,&GamePropertyInfo,wHeadSize);

		return false;
	}

	return true;
}

//��ͨ����
bool CDataBaseEngineSink::OnRequestUserEnableInsure(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_UserEnableInsure));
		if (wDataSize!=sizeof(DBR_GR_UserEnableInsure)) return false;

		//������
		DBR_GR_UserEnableInsure * pUserEnableInsure=(DBR_GR_UserEnableInsure *)pData;

		//���ñ�ʶ
		dwUserID=pUserEnableInsure->dwUserID;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pUserEnableInsure->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pUserEnableInsure->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@strLogonPass"),pUserEnableInsure->szLogonPass);
		m_TreasureDBAide.AddParameter(TEXT("@strInsurePass"),pUserEnableInsure->szInsurePass);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pUserEnableInsure->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_UserEnableInsure"),true);

		//�������
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//�������
		DBO_GR_UserInsureEnableResult UserEnableInsureResult;
		ZeroMemory(&UserEnableInsureResult,sizeof(UserEnableInsureResult));

		//���ñ���
		UserEnableInsureResult.cbActivityGame=pUserEnableInsure->cbActivityGame;
		UserEnableInsureResult.cbInsureEnabled=(lResultCode==DB_SUCCESS)?TRUE:FALSE;
		lstrcpyn(UserEnableInsureResult.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(UserEnableInsureResult.szDescribeString));

		//���ͽ��
		WORD wDataSize=CountStringBuffer(UserEnableInsureResult.szDescribeString);
		WORD wHeadSize=sizeof(UserEnableInsureResult)-sizeof(UserEnableInsureResult.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_INSURE_ENABLE_RESULT,dwContextID,&UserEnableInsureResult,wHeadSize+wDataSize);
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		DBO_GR_UserInsureEnableResult UserEnableInsureResult;
		ZeroMemory(&UserEnableInsureResult,sizeof(UserEnableInsureResult));

		//������
		DBR_GR_UserEnableInsure * pUserEnableInsure=(DBR_GR_UserEnableInsure *)pData;

		//���ñ���
		UserEnableInsureResult.cbInsureEnabled=FALSE;
		UserEnableInsureResult.cbActivityGame=pUserEnableInsure->cbActivityGame;
		lstrcpyn(UserEnableInsureResult.szDescribeString,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),CountArray(UserEnableInsureResult.szDescribeString));

		//���ͽ��
		WORD wDataSize=CountStringBuffer(UserEnableInsureResult.szDescribeString);
		WORD wHeadSize=sizeof(UserEnableInsureResult)-sizeof(UserEnableInsureResult.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_INSURE_ENABLE_RESULT,dwContextID,&UserEnableInsureResult,wHeadSize+wDataSize);

		return false;
	}
}

//������Ϸ��
bool CDataBaseEngineSink::OnRequestUserSaveScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_UserSaveScore));
	if (wDataSize!=sizeof(DBR_GR_UserSaveScore)) return false;

	//��������
	DBR_GR_UserSaveScore * pUserSaveScore=(DBR_GR_UserSaveScore *)pData;
	dwUserID=pUserSaveScore->dwUserID;

	//������
	try
	{
		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pUserSaveScore->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pUserSaveScore->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@lSaveScore"),pUserSaveScore->lSaveScore);
		m_TreasureDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_TreasureDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pUserSaveScore->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_UserSaveScore"),true);

		//�������
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnInsureDisposeResult(dwContextID,lResultCode,pUserSaveScore->lSaveScore,CW2CT(DBVarValue.bstrVal),false,pUserSaveScore->cbActivityGame);
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		OnInsureDisposeResult(dwContextID,DB_ERROR,pUserSaveScore->lSaveScore,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false,pUserSaveScore->cbActivityGame);

		return false;
	}

	return true;
}

//��ȡ��Ϸ��
bool CDataBaseEngineSink::OnRequestUserTakeScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_UserTakeScore));
	if (wDataSize!=sizeof(DBR_GR_UserTakeScore)) return false;

	//��������
	DBR_GR_UserTakeScore * pUserTakeScore=(DBR_GR_UserTakeScore *)pData;
	dwUserID=pUserTakeScore->dwUserID;

	//������
	try
	{
		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pUserTakeScore->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pUserTakeScore->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@lTakeScore"),pUserTakeScore->lTakeScore);
		m_TreasureDBAide.AddParameter(TEXT("@strPassword"),pUserTakeScore->szPassword);
		m_TreasureDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_TreasureDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pUserTakeScore->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_UserTakeScore"),true);

		//�������
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnInsureDisposeResult(dwContextID,lResultCode,0L,CW2CT(DBVarValue.bstrVal),false,pUserTakeScore->cbActivityGame);
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false,pUserTakeScore->cbActivityGame);

		return false;
	}

	return true;
}

//ת����Ϸ��
bool CDataBaseEngineSink::OnRequestUserTransferScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_UserTransferScore));
	if (wDataSize!=sizeof(DBR_GR_UserTransferScore)) return false;

	//��������
	DBR_GR_UserTransferScore * pUserTransferScore=(DBR_GR_UserTransferScore *)pData;
	dwUserID=pUserTransferScore->dwUserID;

	//������
	try
	{
		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pUserTransferScore->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pUserTransferScore->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@lTransferScore"),pUserTransferScore->lTransferScore);
		m_TreasureDBAide.AddParameter(TEXT("@strPassword"),pUserTransferScore->szPassword);
		m_TreasureDBAide.AddParameter(TEXT("@strNickName"),pUserTransferScore->szAccounts);
		m_TreasureDBAide.AddParameter(TEXT("@strTransRemark"),pUserTransferScore->szTransRemark);
		m_TreasureDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_TreasureDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pUserTransferScore->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_UserTransferScore"),true);

		//�������
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnInsureDisposeResult(dwContextID,lResultCode,0L,CW2CT(DBVarValue.bstrVal),false,pUserTransferScore->cbActivityGame);
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false,pUserTransferScore->cbActivityGame);
		
		return false;
	}

	return true;
}

//��ѯ����
bool CDataBaseEngineSink::OnRequestQueryInsureInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_QueryInsureInfo));
	if (wDataSize!=sizeof(DBR_GR_QueryInsureInfo)) return false;

	//������
	DBR_GR_QueryInsureInfo * pQueryInsureInfo=(DBR_GR_QueryInsureInfo *)pData;
	dwUserID=pQueryInsureInfo->dwUserID;

	try
	{
		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pQueryInsureInfo->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pQueryInsureInfo->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@strPassword"),pQueryInsureInfo->szPassword);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//�������
		if (m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_QueryUserInsureInfo"),true)==DB_SUCCESS)
		{
			//��������
			DBO_GR_UserInsureInfo UserInsureInfo;
			ZeroMemory(&UserInsureInfo,sizeof(UserInsureInfo));

			//������Ϣ
			UserInsureInfo.cbActivityGame=pQueryInsureInfo->cbActivityGame;
			UserInsureInfo.cbEnjoinTransfer=m_TreasureDBAide.GetValue_BYTE(TEXT("EnjoinTransfer"));
			UserInsureInfo.wRevenueTake=m_TreasureDBAide.GetValue_WORD(TEXT("RevenueTake"));
			UserInsureInfo.wRevenueTransfer=m_TreasureDBAide.GetValue_WORD(TEXT("RevenueTransfer"));
			UserInsureInfo.wRevenueTransferMember=m_TreasureDBAide.GetValue_WORD(TEXT("RevenueTransferMember"));
			UserInsureInfo.wServerID=m_TreasureDBAide.GetValue_WORD(TEXT("ServerID"));
			UserInsureInfo.lUserScore=m_TreasureDBAide.GetValue_LONGLONG(TEXT("Score"));
			UserInsureInfo.lUserInsure=m_TreasureDBAide.GetValue_LONGLONG(TEXT("Insure"));
			UserInsureInfo.lTransferPrerequisite=m_TreasureDBAide.GetValue_LONGLONG(TEXT("TransferPrerequisite"));

			//���ͽ��
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_INSURE_INFO,dwContextID,&UserInsureInfo,sizeof(UserInsureInfo));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//������
			OnInsureDisposeResult(dwContextID,m_TreasureDBAide.GetReturnValue(),0L,CW2CT(DBVarValue.bstrVal),false,pQueryInsureInfo->cbActivityGame);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false,pQueryInsureInfo->cbActivityGame);
		
		return false;
	}

	return true;
}

//��ѯ�û�
bool CDataBaseEngineSink::OnRequestQueryTransferUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_QueryTransferUserInfo));
	if (wDataSize!=sizeof(DBR_GR_QueryTransferUserInfo)) return false;

	//������
	DBR_GR_QueryTransferUserInfo * pQueryTransferUserInfo=(DBR_GR_QueryTransferUserInfo *)pData;
	dwUserID=pQueryTransferUserInfo->dwUserID;

	try
	{
		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@cbByNickName"),pQueryTransferUserInfo->cbByNickName);
		m_TreasureDBAide.AddParameter(TEXT("@strAccounts"),pQueryTransferUserInfo->szAccounts);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//�������
		if (m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_QueryTransferUserInfo"),true)==DB_SUCCESS)
		{
			//��������
			DBO_GR_UserTransferUserInfo TransferUserInfo;
			ZeroMemory(&TransferUserInfo,sizeof(TransferUserInfo));

			//������Ϣ
			TransferUserInfo.cbActivityGame=pQueryTransferUserInfo->cbActivityGame;
			TransferUserInfo.dwGameID=m_TreasureDBAide.GetValue_DWORD(TEXT("GameID"));
			m_TreasureDBAide.GetValue_String(TEXT("Accounts"), TransferUserInfo.szAccounts, CountArray(TransferUserInfo.szAccounts));

			//���ͽ��
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_INSURE_USER_INFO,dwContextID,&TransferUserInfo,sizeof(TransferUserInfo));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//������
			OnInsureDisposeResult(dwContextID,m_TreasureDBAide.GetReturnValue(),0L,CW2CT(DBVarValue.bstrVal),false,pQueryTransferUserInfo->cbActivityGame);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false,pQueryTransferUserInfo->cbActivityGame);

		return false;
	}

	return true;
}

//��ȡ����
bool CDataBaseEngineSink::OnRequestTaskTake(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_TaskTake));
		if (wDataSize!=sizeof(DBR_GR_TaskTake)) return false;

		//������
		DBR_GR_TaskTake * pTaskTake=(DBR_GR_TaskTake *)pData;

		//���ñ���
		dwUserID = pTaskTake->dwUserID;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pTaskTake->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pTaskTake->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@wTaskID"),pTaskTake->wTaskID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pTaskTake->szPassword);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),pTaskTake->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�нű�
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_TaskTake"),false);

		//��������
		DBO_GR_TaskResult TaskResult;
		ZeroMemory(&TaskResult,sizeof(TaskResult));

		//��ȡ����
		CDBVarValue DBVarValue;
		m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//������Ϣ
		TaskResult.wCommandID=SUB_GR_TASK_TAKE;
		TaskResult.wCurrTaskID=pTaskTake->wTaskID;		
		TaskResult.bSuccessed=lResultCode==DB_SUCCESS;	
		lstrcpyn(TaskResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(TaskResult.szNotifyContent));

		//���ͽ��
		WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_RESULT,dwContextID,&TaskResult,wSendSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GR_TaskResult TaskResult;
		TaskResult.bSuccessed=false;
		TaskResult.wCommandID=SUB_GR_TASK_TAKE;
		lstrcpyn(TaskResult.szNotifyContent,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),CountArray(TaskResult.szNotifyContent));

		//���ͽ��
		WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_RESULT,dwContextID,&TaskResult,wSendSize);

		return false;
	}
}

//��ȡ����
bool CDataBaseEngineSink::OnRequestTaskReward(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_TaskReward));
		if (wDataSize!=sizeof(DBR_GR_TaskReward)) return false;

		//������
		DBR_GR_TaskReward * pTaskReward=(DBR_GR_TaskReward *)pData;

		//���ñ���
		dwUserID = pTaskReward->dwUserID;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pTaskReward->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pTaskReward->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@wTaskID"),pTaskReward->wTaskID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pTaskReward->szPassword);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),pTaskReward->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�нű�
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_TaskReward"),true);

		//��������
		DBO_GR_TaskResult TaskResult;
		ZeroMemory(&TaskResult,sizeof(TaskResult));

		//��ȡ����
		CDBVarValue DBVarValue;
		m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//������Ϣ
		TaskResult.wCommandID=SUB_GR_TASK_REWARD;
		TaskResult.wCurrTaskID=pTaskReward->wTaskID;	
		TaskResult.bSuccessed=lResultCode==DB_SUCCESS;
		lstrcpyn(TaskResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(TaskResult.szNotifyContent));

		//��ȡ����
		if(TaskResult.bSuccessed==true)
		{
			TaskResult.lCurrScore = m_PlatformDBAide.GetValue_LONGLONG(TEXT("Score"));
			TaskResult.lCurrIngot= m_PlatformDBAide.GetValue_LONGLONG(TEXT("Ingot"));
		}

		//���ͽ��
		WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_RESULT,dwContextID,&TaskResult,wSendSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		
		//����ṹ
		DBO_GR_TaskResult TaskResult;
		TaskResult.bSuccessed=false;
		TaskResult.wCommandID=SUB_GR_TASK_REWARD;
		lstrcpyn(TaskResult.szNotifyContent,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),CountArray(TaskResult.szNotifyContent));

		//���ͽ��
		WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_RESULT,dwContextID,&TaskResult,wSendSize);

		return false;
	}
}

//��������
bool CDataBaseEngineSink::OnRequestLoadTaskList(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//���ñ���
		dwUserID = 0;

		//�������
		m_PlatformDBAide.ResetParameter();

		//ִ������
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_LoadTaskList"),true);

		//ִ�гɹ�
		if(lResultCode==DB_SUCCESS)
		{
			//��������
			tagTaskParameter TaskParameter;	
			BYTE cbDataBuffer[SOCKET_TCP_PACKET-1024]={0};					
			DBO_GR_TaskListInfo * pTaskListInfo = (DBO_GR_TaskListInfo *)cbDataBuffer;
			LPBYTE pDataBuffer=cbDataBuffer+sizeof(DBO_GR_TaskListInfo);

			//���ñ���
			WORD wTaskCount=0;
			WORD wSendDataSize=sizeof(DBO_GR_TaskListInfo);		

			//��������
			while (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				//��ȡ����
				TaskParameter.wTaskID = m_PlatformDBAide.GetValue_WORD(TEXT("TaskID"));
				TaskParameter.wTaskType = m_PlatformDBAide.GetValue_WORD(TEXT("TaskType"));
				TaskParameter.cbPlayerType = m_PlatformDBAide.GetValue_BYTE(TEXT("UserType"));
				TaskParameter.wKindID = m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
				TaskParameter.wTaskObject= m_PlatformDBAide.GetValue_WORD(TEXT("Innings"));
				TaskParameter.dwTimeLimit = m_PlatformDBAide.GetValue_DWORD(TEXT("TimeLimit"));
				TaskParameter.lStandardAwardGold = m_PlatformDBAide.GetValue_LONGLONG(TEXT("StandardAwardGold"));
				TaskParameter.lStandardAwardMedal = m_PlatformDBAide.GetValue_LONGLONG(TEXT("StandardAwardMedal"));
				TaskParameter.lMemberAwardGold = m_PlatformDBAide.GetValue_LONGLONG(TEXT("MemberAwardGold"));
				TaskParameter.lMemberAwardMedal = m_PlatformDBAide.GetValue_LONGLONG(TEXT("MemberAwardMedal"));

				//������Ϣ
				m_PlatformDBAide.GetValue_String(TEXT("TaskName"),TaskParameter.szTaskName,CountArray(TaskParameter.szTaskName));
				m_PlatformDBAide.GetValue_String(TEXT("TaskDescription"),TaskParameter.szTaskDescribe,CountArray(TaskParameter.szTaskDescribe));

				//�����ж�				
				if(wSendDataSize+sizeof(tagTaskParameter)>sizeof(cbDataBuffer))
				{
					//���ñ���
					pTaskListInfo->wTaskCount = wTaskCount;

					//��������
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_LIST,dwContextID,pTaskListInfo,wSendDataSize);

					//���ñ���
					wTaskCount=0;
					wSendDataSize=sizeof(DBO_GR_TaskListInfo);	
					pDataBuffer = cbDataBuffer+sizeof(DBO_GR_TaskListInfo);
				}

				//��������
				CopyMemory(pDataBuffer,&TaskParameter,sizeof(tagTaskParameter));

				//���ñ���
				wTaskCount++;
				wSendDataSize += sizeof(tagTaskParameter);
				pDataBuffer += sizeof(tagTaskParameter);

				//�ƶ���¼
				m_PlatformDBModule->MoveToNext();
			}

			//ʣ�෢��
			if(wTaskCount>0 && wSendDataSize>0)
			{
				//���ñ���
				pTaskListInfo->wTaskCount = wTaskCount;

				//��������
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_LIST,dwContextID,pTaskListInfo,wSendDataSize);
			}

			//����֪ͨ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_LIST_END,dwContextID,NULL,0);						
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		
		return false;
	}

	return true;
}

//��ѯ����
bool CDataBaseEngineSink::OnRequestTaskQueryInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_TaskQueryInfo));
		if (wDataSize!=sizeof(DBR_GR_TaskQueryInfo)) return false;

		//������
		DBR_GR_TaskQueryInfo * pTaskQueryInfo=(DBR_GR_TaskQueryInfo *)pData;

		//���ñ���
		dwUserID = pTaskQueryInfo->dwUserID;

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wKindID"),0);
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pTaskQueryInfo->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pTaskQueryInfo->szPassword);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�нű�
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_QueryTaskInfo"),true);

		//ִ�гɹ�
		if(lResultCode==DB_SUCCESS)
		{
			//��������
			DBO_GR_TaskInfo TaskInfo;
			tagTaskStatus * pTaskStatus=NULL;
			ZeroMemory(&TaskInfo,sizeof(TaskInfo));

			//��������
			while (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				//���ñ���
				pTaskStatus = &TaskInfo.TaskStatus[TaskInfo.wTaskCount++];

				//��ȡ����
				pTaskStatus->wTaskID = m_PlatformDBAide.GetValue_WORD(TEXT("TaskID"));
				pTaskStatus->cbTaskStatus = m_PlatformDBAide.GetValue_BYTE(TEXT("TaskStatus"));
				pTaskStatus->wTaskProgress = m_PlatformDBAide.GetValue_WORD(TEXT("Progress"));

				//�ƶ���¼
				m_PlatformDBModule->MoveToNext();
			}

			//���ͽ��
			WORD wSendDataSize = sizeof(TaskInfo)-sizeof(TaskInfo.TaskStatus);
			wSendDataSize += sizeof(TaskInfo.TaskStatus[0])*TaskInfo.wTaskCount;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_INFO,dwContextID,&TaskInfo,wSendDataSize);
		}
		else
		{
			//��������
			DBO_GR_TaskResult TaskResult;
			ZeroMemory(&TaskResult,sizeof(TaskResult));

			//��ȡ����
			CDBVarValue DBVarValue;
			m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//������Ϣ
			TaskResult.bSuccessed=false;
			TaskResult.wCommandID=SUB_GR_TASK_LOAD_INFO;
			lstrcpyn(TaskResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(TaskResult.szNotifyContent));

			//���ͽ��
			WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_RESULT,dwContextID,&TaskResult,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		
		//����ṹ
		DBO_GR_TaskResult TaskResult;
		TaskResult.bSuccessed=false;
		TaskResult.wCommandID=SUB_GR_TASK_LOAD_INFO;
		lstrcpyn(TaskResult.szNotifyContent,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),CountArray(TaskResult.szNotifyContent));

		//���ͽ��
		WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_RESULT,dwContextID,&TaskResult,wSendSize);

		return false;
	}
}


//��Ա����
bool CDataBaseEngineSink::OnRequestLoadMemberParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//���ñ���
		dwUserID = 0;

		//�������
		m_TreasureDBAide.ResetParameter();

		//ִ������
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_LoadMemberParameter"),true);

		//ִ�гɹ�
		if(lResultCode==DB_SUCCESS)
		{
			//��������
			DBO_GR_MemberParameter MemberParameter;
			ZeroMemory(&MemberParameter,sizeof(MemberParameter));

			//���ñ���
			WORD wMemberCount=0;
			tagMemberParameter * pMemberParameter=NULL;

			//��������
			while (m_TreasureDBModule->IsRecordsetEnd()==false)
			{
				//���ñ���
				pMemberParameter = &MemberParameter.MemberParameter[MemberParameter.wMemberCount++];

				//��ȡ����
				pMemberParameter->cbMemberOrder= m_TreasureDBAide.GetValue_BYTE(TEXT("MemberOrder"));
				pMemberParameter->lMemberPrice= m_TreasureDBAide.GetValue_LONGLONG(TEXT("MemberPrice"));
				pMemberParameter->lPresentScore= m_TreasureDBAide.GetValue_LONGLONG(TEXT("PresentScore"));
				m_TreasureDBAide.GetValue_String(TEXT("MemberName"),pMemberParameter->szMemberName,CountArray(pMemberParameter->szMemberName));

				//�ƶ���¼
				m_TreasureDBModule->MoveToNext();
			}

			//��������
			WORD wSendDataSize = sizeof(MemberParameter)-sizeof(MemberParameter.MemberParameter);
			wSendDataSize += sizeof(tagMemberParameter)*MemberParameter.wMemberCount;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_MEMBER_PAREMETER,dwContextID,&MemberParameter,wSendDataSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//�����Ա
bool CDataBaseEngineSink::OnRequestPurchaseMember(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_PurchaseMember));
		if (wDataSize!=sizeof(DBR_GR_PurchaseMember)) return false;

		//������
		DBR_GR_PurchaseMember * pPurchaseMember=(DBR_GR_PurchaseMember *)pData;

		//���ñ���
		dwUserID = pPurchaseMember->dwUserID;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pPurchaseMember->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pPurchaseMember->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@cbMemberOrder"),pPurchaseMember->cbMemberOrder);
		m_TreasureDBAide.AddParameter(TEXT("@PurchaseTime"),pPurchaseMember->wPurchaseTime);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pPurchaseMember->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strNotifyContent"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�нű�
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_PurchaseMember"),true);

		//����ṹ
		DBO_GR_PurchaseResult PurchaseResult;
		ZeroMemory(&PurchaseResult,sizeof(PurchaseResult));

		//ִ�гɹ�
		if(lResultCode==DB_SUCCESS)
		{
			//���ñ���
			PurchaseResult.bSuccessed=true;

			//��������
			if (m_TreasureDBModule->IsRecordsetEnd()==false)
			{
				//��ȡ����
				PurchaseResult.lCurrScore = m_TreasureDBAide.GetValue_LONGLONG(TEXT("CurrScore"));
				PurchaseResult.dCurrBeans = m_TreasureDBAide.GetValue_DOUBLE(TEXT("CurrBeans"));
				PurchaseResult.dwUserRight = m_TreasureDBAide.GetValue_DWORD(TEXT("UserRight"));
				PurchaseResult.cbMemberOrder = m_TreasureDBAide.GetValue_BYTE(TEXT("MemberOrder"));
			}	
		}

		//��ʾ����
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strNotifyContent"),DBVarValue);
		lstrcpyn(PurchaseResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(PurchaseResult.szNotifyContent));

		//�����С
		WORD wSendDataSize = sizeof(PurchaseResult)-sizeof(PurchaseResult.szNotifyContent);
		wSendDataSize += CountStringBuffer(PurchaseResult.szNotifyContent);

		//���Ͳ���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_PURCHASE_RESULT,dwContextID,&PurchaseResult,wSendDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		
		//����ṹ
		DBO_GR_PurchaseResult PurchaseResult;
		ZeroMemory(&PurchaseResult,sizeof(PurchaseResult));

		//���ñ���
		lstrcpyn(PurchaseResult.szNotifyContent,TEXT("���ݿ��쳣�����Ժ����ԣ�"),CountArray(PurchaseResult.szNotifyContent));

		//�����С
		WORD wSendDataSize = sizeof(PurchaseResult)-sizeof(PurchaseResult.szNotifyContent);
		wSendDataSize += CountStringBuffer(PurchaseResult.szNotifyContent);

		//���Ͳ���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_PURCHASE_RESULT,dwContextID,&PurchaseResult,wSendDataSize);

		return false;
	}
}

//�һ���Ϸ��
bool CDataBaseEngineSink::OnRequestExchangeScoreByIngot(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_ExchangeScoreByIngot));
		if (wDataSize!=sizeof(DBR_GR_ExchangeScoreByIngot)) return false;

		//������
		DBR_GR_ExchangeScoreByIngot * pExchangeScore=(DBR_GR_ExchangeScoreByIngot *)pData;

		//���ñ���
		dwUserID = pExchangeScore->dwUserID;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pExchangeScore->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pExchangeScore->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@ExchangeIngot"),pExchangeScore->lExchangeIngot);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pExchangeScore->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strNotifyContent"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�нű�
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_ExchangeScoreByIngot"),true);

		//����ṹ
		DBO_GR_ExchangeResult ExchangeResult;
		ZeroMemory(&ExchangeResult,sizeof(ExchangeResult));

		//ִ�гɹ�
		if(lResultCode==DB_SUCCESS)
		{
			//���ñ���
			ExchangeResult.bSuccessed=true;

			//��������
			if (m_TreasureDBModule->IsRecordsetEnd()==false)
			{
				//��ȡ����
				ExchangeResult.lCurrScore = m_TreasureDBAide.GetValue_LONGLONG(TEXT("CurrScore"));
				ExchangeResult.lCurrInsure = m_TreasureDBAide.GetValue_LONGLONG(TEXT("CurrInsure"));
				ExchangeResult.lCurrIngot = m_TreasureDBAide.GetValue_LONGLONG(TEXT("CurrIngot"));
				ExchangeResult.dCurrBeans = m_TreasureDBAide.GetValue_DOUBLE(TEXT("CurrBeans"));
			}	
		}

		//��ʾ����
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strNotifyContent"),DBVarValue);
		lstrcpyn(ExchangeResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(ExchangeResult.szNotifyContent));

		//�����С
		WORD wSendDataSize = sizeof(ExchangeResult)-sizeof(ExchangeResult.szNotifyContent);
		wSendDataSize += CountStringBuffer(ExchangeResult.szNotifyContent);

		//���Ͳ���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_EXCHANGE_RESULT,dwContextID,&ExchangeResult,wSendDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		
		//����ṹ
		DBO_GR_ExchangeResult ExchangeResult;
		ZeroMemory(&ExchangeResult,sizeof(ExchangeResult));

		//���ñ���
		lstrcpyn(ExchangeResult.szNotifyContent,TEXT("���ݿ��쳣�����Ժ����ԣ�"),CountArray(ExchangeResult.szNotifyContent));

		//�����С
		WORD wSendDataSize = sizeof(ExchangeResult)-sizeof(ExchangeResult.szNotifyContent);
		wSendDataSize += CountStringBuffer(ExchangeResult.szNotifyContent);

		//���Ͳ���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_EXCHANGE_RESULT,dwContextID,&ExchangeResult,wSendDataSize);

		return false;
	}
}

//�һ���Ϸ��
bool CDataBaseEngineSink::OnRequestExchangeScoreByBeans(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_ExchangeScoreByBeans));
		if (wDataSize!=sizeof(DBR_GR_ExchangeScoreByBeans)) return false;

		//������
		DBR_GR_ExchangeScoreByBeans * pExchangeScore=(DBR_GR_ExchangeScoreByBeans *)pData;

		//���ñ���
		dwUserID = pExchangeScore->dwUserID;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pExchangeScore->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pExchangeScore->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@ExchangeBeans"),pExchangeScore->dExchangeBeans);
		m_TreasureDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_TreasureDBAide.AddParameter(TEXT("@strMachineID"),pExchangeScore->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strNotifyContent"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�нű�
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_ExchangeScoreByBeans"),true);

		//����ṹ
		DBO_GR_ExchangeResult ExchangeResult;
		ZeroMemory(&ExchangeResult,sizeof(ExchangeResult));

		//ִ�гɹ�
		if(lResultCode==DB_SUCCESS)
		{
			//���ñ���
			ExchangeResult.bSuccessed=true;

			//��������
			if (m_TreasureDBModule->IsRecordsetEnd()==false)
			{
				//��ȡ����
				ExchangeResult.lCurrScore = m_TreasureDBAide.GetValue_LONGLONG(TEXT("CurrScore"));
				ExchangeResult.lCurrInsure = m_TreasureDBAide.GetValue_LONGLONG(TEXT("CurrInsure"));
				ExchangeResult.lCurrIngot = m_TreasureDBAide.GetValue_LONGLONG(TEXT("CurrIngot"));
				ExchangeResult.dCurrBeans = m_TreasureDBAide.GetValue_DOUBLE(TEXT("CurrBeans"));
			}	
		}

		//��ʾ����
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strNotifyContent"),DBVarValue);
		lstrcpyn(ExchangeResult.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(ExchangeResult.szNotifyContent));

		//�����С
		WORD wSendDataSize = sizeof(ExchangeResult)-sizeof(ExchangeResult.szNotifyContent);
		wSendDataSize += CountStringBuffer(ExchangeResult.szNotifyContent);

		//���Ͳ���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_EXCHANGE_RESULT,dwContextID,&ExchangeResult,wSendDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		
		//����ṹ
		DBO_GR_ExchangeResult ExchangeResult;
		ZeroMemory(&ExchangeResult,sizeof(ExchangeResult));

		//���ñ���
		lstrcpyn(ExchangeResult.szNotifyContent,TEXT("���ݿ��쳣�����Ժ����ԣ�"),CountArray(ExchangeResult.szNotifyContent));

		//�����С
		WORD wSendDataSize = sizeof(ExchangeResult)-sizeof(ExchangeResult.szNotifyContent);
		wSendDataSize += CountStringBuffer(ExchangeResult.szNotifyContent);

		//���Ͳ���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_EXCHANGE_RESULT,dwContextID,&ExchangeResult,wSendDataSize);

		return false;
	}
}

//��������
bool CDataBaseEngineSink::OnRequestPropertyRequest(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_PropertyRequest));
		if (wDataSize!=sizeof(DBR_GR_PropertyRequest)) return false;

		//������
		DBR_GR_PropertyRequest * pPropertyRequest=(DBR_GR_PropertyRequest *)pData;
		dwUserID=pPropertyRequest->dwSourceUserID;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pPropertyRequest->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_GameDBAide.ResetParameter();

		//������Ϣ
		m_GameDBAide.AddParameter(TEXT("@dwSourceUserID"),pPropertyRequest->dwSourceUserID);
		m_GameDBAide.AddParameter(TEXT("@dwTargetUserID"),pPropertyRequest->dwTargetUserID);
		m_GameDBAide.AddParameter(TEXT("@wPropertyID"),pPropertyRequest->wPropertyIndex);
		m_GameDBAide.AddParameter(TEXT("@wPropertyCount"),pPropertyRequest->wItemCount);

		//��������
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameter(TEXT("@wTableID"),pPropertyRequest->wTableID);

		//����ʽ
		m_GameDBAide.AddParameter(TEXT("@cbConsumeScore"),pPropertyRequest->cbConsumeScore);
		m_GameDBAide.AddParameter(TEXT("@lFrozenedScore"),pPropertyRequest->lFrozenedScore);

		//ϵͳ��Ϣ
		m_GameDBAide.AddParameter(TEXT("@dwEnterID"),pPropertyRequest->dwInoutIndex);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_GameDBAide.AddParameter(TEXT("@strMachineID"),pPropertyRequest->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//�������
		if (m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_ConsumeProperty"),true)==DB_SUCCESS)
		{
			//��������
			DBO_GR_S_PropertySuccess PresentSuccess;
			ZeroMemory(&PresentSuccess,sizeof(PresentSuccess));

			//������Ϣ
			PresentSuccess.cbRequestArea=pPropertyRequest->cbRequestArea;
			PresentSuccess.wItemCount=pPropertyRequest->wItemCount;
			PresentSuccess.wPropertyIndex=pPropertyRequest->wPropertyIndex;
			PresentSuccess.dwSourceUserID=pPropertyRequest->dwSourceUserID;
			PresentSuccess.dwTargetUserID=pPropertyRequest->dwTargetUserID;

			//����ģʽ
			PresentSuccess.cbConsumeScore=pPropertyRequest->cbConsumeScore;
			PresentSuccess.lFrozenedScore=pPropertyRequest->lFrozenedScore;

			//�û�Ȩ��
			PresentSuccess.dwUserRight=pPropertyRequest->dwUserRight;

			//�����Ϣ
			PresentSuccess.lConsumeGold=m_GameDBAide.GetValue_LONGLONG(TEXT("ConsumeGold"));
			PresentSuccess.lSendLoveLiness=m_GameDBAide.GetValue_LONG(TEXT("SendLoveLiness"));
			PresentSuccess.lRecvLoveLiness=m_GameDBAide.GetValue_LONG(TEXT("RecvLoveLiness"));
			PresentSuccess.cbMemberOrder=m_GameDBAide.GetValue_BYTE(TEXT("MemberOrder"));

			//���ͽ��
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_PROPERTY_SUCCESS,dwContextID,&PresentSuccess,sizeof(PresentSuccess));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_GameDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//��������
			DBO_GR_PropertyFailure PropertyFailure;
			ZeroMemory(&PropertyFailure,sizeof(PropertyFailure));

			//���ñ���
			PropertyFailure.lResultCode=m_GameDBAide.GetReturnValue();
			PropertyFailure.lFrozenedScore=pPropertyRequest->lFrozenedScore;
			PropertyFailure.cbRequestArea=pPropertyRequest->cbRequestArea;
			lstrcpyn(PropertyFailure.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(PropertyFailure.szDescribeString));

			//���ͽ��
			WORD wDataSize=CountStringBuffer(PropertyFailure.szDescribeString);
			WORD wHeadSize=sizeof(PropertyFailure)-sizeof(PropertyFailure.szDescribeString);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_PROPERTY_FAILURE,dwContextID,&PropertyFailure,wHeadSize+wDataSize);
		}
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		
		//��������
		DBO_GR_PropertyFailure PropertyFailure;
		ZeroMemory(&PropertyFailure,sizeof(PropertyFailure));

		//��������
		DBR_GR_PropertyRequest * pPropertyRequest=(DBR_GR_PropertyRequest *)pData;

		//���ñ���
		PropertyFailure.lResultCode=DB_ERROR;
		PropertyFailure.lFrozenedScore=pPropertyRequest->lFrozenedScore;
		PropertyFailure.cbRequestArea=pPropertyRequest->cbRequestArea;
		lstrcpyn(PropertyFailure.szDescribeString,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),CountArray(PropertyFailure.szDescribeString));

		//���ͽ��
		WORD wDataSize=CountStringBuffer(PropertyFailure.szDescribeString);
		WORD wHeadSize=sizeof(PropertyFailure)-sizeof(PropertyFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_PROPERTY_FAILURE,dwContextID,&PropertyFailure,wHeadSize+wDataSize);

		return false;
	}

	return true;
}

//�û���Ϸ����
bool CDataBaseEngineSink::OnRequestWriteUserGameData(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//������
		DBR_GR_WriteUserGameData * pPropertyRequest=(DBR_GR_WriteUserGameData *)pData;
		dwUserID=pPropertyRequest->dwUserID;

		//Ч�����
		WORD wDataSize1=CountStringBuffer(pPropertyRequest->szUserGameData);
		WORD wHeadSize1=sizeof(DBR_GR_WriteUserGameData)-sizeof(pPropertyRequest->szUserGameData);

		ASSERT(wDataSize1<wDataSize && wDataSize==wDataSize1+wHeadSize1);
		if (wDataSize1>=wDataSize && wDataSize!=wDataSize1+wHeadSize1) return false;		

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pPropertyRequest->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBAide.ResetParameter();

		//������Ϣ
		m_TreasureDBAide.AddParameter(TEXT("@wKindID"),pPropertyRequest->wKindID);
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pPropertyRequest->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@strUserGameData"),pPropertyRequest->szUserGameData);

		//�������
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_WriteUserGameData"),false);
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		
		return false;
	}

	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestMatchSignup(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_MatchSignup));
	if (wDataSize!=sizeof(DBR_GR_MatchSignup)) return false;

	//��������
	DBR_GR_MatchSignup * pMatchSignup=(DBR_GR_MatchSignup *)pData;
	dwUserID=pMatchSignup->dwUserID;

	//������
	try
	{
		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pMatchSignup->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pMatchSignup->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@lMatchFee"),pMatchSignup->lMatchFee);
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);		
		m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pMatchSignup->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@dwMatchNo"),pMatchSignup->dwMatchNO);
		m_GameDBAide.AddParameter(TEXT("@cbMatchType"),pMatchSignup->cbMatchType);	
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_GameDBAide.AddParameter(TEXT("@strMachineID"),pMatchSignup->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//�������
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_UserMatchFee"),true);

		//����ṹ
		DBO_GR_MatchSingupResult MatchSignupResult;
		ZeroMemory(&MatchSignupResult,sizeof(MatchSignupResult));

		//���ñ���
		MatchSignupResult.dwMatchNO=pMatchSignup->dwMatchNO;
		MatchSignupResult.bResultCode=lReturnValue==DB_SUCCESS;		

		//��ȡ�Ƹ�
		if(MatchSignupResult.bResultCode==true)
		{
			MatchSignupResult.lCurrGold=m_GameDBAide.GetValue_LONGLONG(TEXT("Score"));
			MatchSignupResult.lCurrIngot=m_GameDBAide.GetValue_LONGLONG(TEXT("Ingot"));
		}
		else
		{
			//��������
			CDBVarValue DBVarValue;
			m_GameDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
			lstrcpyn(MatchSignupResult.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(MatchSignupResult.szDescribeString));
		}

		//���ͽ��
		WORD wSendDataSize=sizeof(MatchSignupResult)-sizeof(MatchSignupResult.szDescribeString);
		wSendDataSize+=CountStringBuffer(MatchSignupResult.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_SIGNUP_RESULT,dwContextID,&MatchSignupResult,wSendDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GR_MatchSingupResult MatchSignupResult;
		ZeroMemory(&MatchSignupResult,sizeof(MatchSignupResult));

		//���ñ���		
		MatchSignupResult.bResultCode=false;
		MatchSignupResult.dwMatchNO=pMatchSignup->dwMatchNO;
		lstrcpyn(MatchSignupResult.szDescribeString,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),CountArray(MatchSignupResult.szDescribeString));

		//���ͽ��
		WORD wSendDataSize=sizeof(MatchSignupResult)-sizeof(MatchSignupResult.szDescribeString);
		wSendDataSize+=CountStringBuffer(MatchSignupResult.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_SIGNUP_RESULT,dwContextID,&MatchSignupResult,wSendDataSize);

		return false;
	}

	return true;
}

//�˳�����
bool CDataBaseEngineSink::OnRequestMatchUnSignup(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_MatchUnSignup));
	if (wDataSize!=sizeof(DBR_GR_MatchUnSignup)) return false;

	//��������
	DBR_GR_MatchUnSignup * pMatchUnSignup=(DBR_GR_MatchUnSignup *)pData;
	dwUserID=pMatchUnSignup->dwUserID;

	//������
	try
	{
		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pMatchUnSignup->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pMatchUnSignup->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@lMatchFee"),pMatchUnSignup->lMatchFee);
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);		
		m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pMatchUnSignup->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@dwMatchNO"),pMatchUnSignup->dwMatchNo);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_GameDBAide.AddParameter(TEXT("@strMachineID"),pMatchUnSignup->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//�������
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_UserMatchQuit"),true);

		//����ṹ
		DBO_GR_MatchSingupResult MatchSignupResult;
		ZeroMemory(&MatchSignupResult,sizeof(MatchSignupResult));

		//���ñ���
		MatchSignupResult.dwReason=pMatchUnSignup->dwReason;
		MatchSignupResult.dwMatchNO=pMatchUnSignup->dwMatchNo;
		MatchSignupResult.bResultCode=lReturnValue==DB_SUCCESS;

		//��ȡ�Ƹ�
		if(MatchSignupResult.bResultCode==true)
		{
			MatchSignupResult.lCurrGold=m_GameDBAide.GetValue_LONGLONG(TEXT("Score"));
			MatchSignupResult.lCurrIngot=m_GameDBAide.GetValue_LONGLONG(TEXT("Ingot"));
		}
		else
		{
			//��������
			CDBVarValue DBVarValue;
			m_GameDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
			lstrcpyn(MatchSignupResult.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(MatchSignupResult.szDescribeString));
		}

		//���ͽ��
		WORD wSendDataSize=sizeof(MatchSignupResult)-sizeof(MatchSignupResult.szDescribeString);
		wSendDataSize+=CountStringBuffer(MatchSignupResult.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_UNSIGNUP_RESULT,dwContextID,&MatchSignupResult,wSendDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		
		//����ṹ
		DBO_GR_MatchSingupResult MatchSignupResult;
		ZeroMemory(&MatchSignupResult,sizeof(MatchSignupResult));

		//���ñ���
		MatchSignupResult.bResultCode=false;
		MatchSignupResult.dwReason=pMatchUnSignup->dwReason;
		MatchSignupResult.dwMatchNO=pMatchUnSignup->dwMatchNo;
		lstrcpyn(MatchSignupResult.szDescribeString,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),CountArray(MatchSignupResult.szDescribeString));

		//���ͽ��
		WORD wSendDataSize=sizeof(MatchSignupResult)-sizeof(MatchSignupResult.szDescribeString);
		wSendDataSize+=CountStringBuffer(MatchSignupResult.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_SIGNUP_RESULT,dwContextID,&MatchSignupResult,wSendDataSize);

		return false;
	}
}

//������ʼ
bool CDataBaseEngineSink::OnRequestMatchStart(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_MatchStart));
	if (wDataSize!=sizeof(DBR_GR_MatchStart)) return false;

	//��������
	DBR_GR_MatchStart * pMatchStart=(DBR_GR_MatchStart *)pData;
	dwUserID=pMatchStart->dwMatchNO;

	//������
	try
	{
		//�������
		m_GameDBAide.ResetParameter();		
		m_GameDBAide.AddParameter(TEXT("@wServerID"),pMatchStart->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@wMatchID"),pMatchStart->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@wMatchNo"),pMatchStart->dwMatchNO);
		m_GameDBAide.AddParameter(TEXT("@cbMatchType"),pMatchStart->cbMatchType);

		//�������
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_MatchStart"),true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		
		//������
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false);

		return false;
	}

	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestMatchOver(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_MatchOver));
	if (wDataSize!=sizeof(DBR_GR_MatchOver)) return false;

	//��������
	DBR_GR_MatchOver * pMatchOver=(DBR_GR_MatchOver *)pData;
	dwUserID=pMatchOver->dwMatchNO;

	//������
	try
	{
		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@wServerID"),pMatchOver->wServerID);	
		m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pMatchOver->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@dwMatchNo"),pMatchOver->dwMatchNO);
		m_GameDBAide.AddParameter(TEXT("@cbMatchType"),pMatchOver->cbMatchType);	
		m_GameDBAide.AddParameter(TEXT("@MatchStartTime"),pMatchOver->MatchStartTime);	
		m_GameDBAide.AddParameter(TEXT("@MatchEndTime"),pMatchOver->MatchEndTime);	

		//�������
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_MatchOver"),true);

		//ִ�гɹ�
		if(lReturnValue==DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_MatchRankList MatchRankList;
			ZeroMemory(&MatchRankList,sizeof(MatchRankList));

			//��������
			tagMatchRankInfo * pMatchRankInfo=NULL;

			//���ñ���
			MatchRankList.dwMatchID=pMatchOver->dwMatchID;
			MatchRankList.dwMatchNO=pMatchOver->dwMatchNO;

			//��ȡ��¼
			while(m_GameDBModule->IsRecordsetEnd()==false)
			{
				pMatchRankInfo = &MatchRankList.MatchRankInfo[MatchRankList.wUserCount++];
				pMatchRankInfo->wRankID=m_GameDBAide.GetValue_WORD(TEXT("RankID"));
				pMatchRankInfo->dwUserID=m_GameDBAide.GetValue_DWORD(TEXT("UserID"));
				pMatchRankInfo->lMatchScore=m_GameDBAide.GetValue_LONGLONG(TEXT("Score"));
				pMatchRankInfo->lRewardGold=m_GameDBAide.GetValue_LONGLONG(TEXT("RewardGold"));
				pMatchRankInfo->dwRewardIngot=m_GameDBAide.GetValue_DWORD(TEXT("RewardMedal"));
				pMatchRankInfo->dwRewardExperience=m_GameDBAide.GetValue_DWORD(TEXT("RewardExperience"));

				//�ƶ��α� 
				m_GameDBModule->MoveToNext();
			}

			//��������
			WORD wSendDataSize = sizeof(MatchRankList)-sizeof(MatchRankList.MatchRankInfo);
			wSendDataSize += MatchRankList.wUserCount*sizeof(MatchRankList.MatchRankInfo[0]);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_RANK_LIST,dwContextID,&MatchRankList,wSendDataSize);
		}
		
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		
		return false;
	}

	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestMatchReward(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_MatchReward));
	if (wDataSize!=sizeof(DBR_GR_MatchReward)) return false;

	//��������
	DBR_GR_MatchReward * pMatchReward=(DBR_GR_MatchReward *)pData;
	dwUserID=pMatchReward->dwUserID;

	//������
	try
	{
		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pMatchReward->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pMatchReward->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@dwRewardGold"),pMatchReward->lRewardGold);
		m_GameDBAide.AddParameter(TEXT("@dwRewardIngot"),pMatchReward->dwRewardIngot);
		m_GameDBAide.AddParameter(TEXT("@dwRewardExperience"),pMatchReward->dwRewardExperience);
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);

		//�������
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_MatchReward"),true);

		//����ṹ
		DBO_GR_MatchRewardResult MatchRewardResult;
		ZeroMemory(&MatchRewardResult,sizeof(MatchRewardResult));

		//���ñ���
		MatchRewardResult.dwUserID=pMatchReward->dwUserID;
		MatchRewardResult.bResultCode=lReturnValue==DB_SUCCESS;

		//��ȡ�Ƹ�
		if(MatchRewardResult.bResultCode==true)
		{
			MatchRewardResult.lCurrGold=m_GameDBAide.GetValue_LONGLONG(TEXT("Score"));
			MatchRewardResult.lCurrIngot=m_GameDBAide.GetValue_LONGLONG(TEXT("Ingot"));
			MatchRewardResult.dwCurrExperience=m_GameDBAide.GetValue_DWORD(TEXT("Experience"));
		}

		//���ͽ��
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_REWARD_RESULT,dwContextID,&MatchRewardResult,sizeof(MatchRewardResult));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		
		//����ṹ
		DBO_GR_MatchRewardResult MatchRewardResult;
		ZeroMemory(&MatchRewardResult,sizeof(MatchRewardResult));

		//���ñ���
		MatchRewardResult.bResultCode=false;

		//���ͽ��
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_REWARD_RESULT,dwContextID,&MatchRewardResult,sizeof(MatchRewardResult));

		return false;
	}

	return true;
}

//������̭
bool CDataBaseEngineSink::OnRequestMatchEliminate(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_MatchEliminate));
	if (wDataSize!=sizeof(DBR_GR_MatchEliminate)) return false;

	//��������
	DBR_GR_MatchEliminate * pMatchEliminate=(DBR_GR_MatchEliminate *)pData;
	dwUserID=pMatchEliminate->dwUserID;

	//������
	try
	{
		//�������
		m_GameDBAide.ResetParameter();		
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pMatchEliminate->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),pMatchEliminate->wServerID);
		m_GameDBAide.AddParameter(TEXT("@wMatchID"),pMatchEliminate->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@wMatchNo"),pMatchEliminate->dwMatchNO);	
		m_GameDBAide.AddParameter(TEXT("@cbMatchType"),pMatchEliminate->cbMatchType);

		//�������
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_MatchEliminate"),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		
		return false;
	}

	return true;
}

//�ȼ�����
bool CDataBaseEngineSink::OnRequestLoadGrowLevelConfig(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		dwUserID=0;

		//�������
		m_PlatformDBAide.ResetParameter();

		//ִ������
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_LoadGrowLevelConfig"),true);

		//ִ�гɹ�
		if(lResultCode==DB_SUCCESS)
		{
			//��������
			DBO_GR_GrowLevelConfig GrowLevelConfig;
			ZeroMemory(&GrowLevelConfig,sizeof(GrowLevelConfig));

			//���ñ���
			WORD wLevelCount=0;
			tagGrowLevelConfig * pGrowLevelConfig=NULL;

			//��������
			while (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				//����ж�
				if(GrowLevelConfig.wLevelCount>=CountArray(GrowLevelConfig.GrowLevelConfig)) break;

				//���ñ���
				pGrowLevelConfig = &GrowLevelConfig.GrowLevelConfig[GrowLevelConfig.wLevelCount++];

				//��ȡ����
				pGrowLevelConfig->wLevelID= m_PlatformDBAide.GetValue_WORD(TEXT("LevelID"));
				pGrowLevelConfig->dwExperience= m_PlatformDBAide.GetValue_DWORD(TEXT("Experience"));

				//�ƶ���¼
				m_PlatformDBModule->MoveToNext();
			}

			//��������
			WORD wSendDataSize = sizeof(GrowLevelConfig)-sizeof(GrowLevelConfig.GrowLevelConfig);
			wSendDataSize += sizeof(tagGrowLevelConfig)*GrowLevelConfig.wLevelCount;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROWLEVEL_CONFIG,dwContextID,&GrowLevelConfig,wSendDataSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		
		return false;
	}

	return true;
}

//��ѯ�ȼ�
bool CDataBaseEngineSink::OnRequestQueryGrowLevelParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_GrowLevelQueryInfo));
		if (wDataSize!=sizeof(DBR_GR_GrowLevelQueryInfo)) return false;

		//������
		DBR_GR_GrowLevelQueryInfo * pGrowLevelQueryInfo=(DBR_GR_GrowLevelQueryInfo *)pData;
		dwUserID=pGrowLevelQueryInfo->dwUserID;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pGrowLevelQueryInfo->dwClientAddr;
		_sntprintf(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),pGrowLevelQueryInfo->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@strPassword"),pGrowLevelQueryInfo->szPassword);
		m_PlatformDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),pGrowLevelQueryInfo->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strUpgradeDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�нű�
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_QueryGrowLevel"),true);

		//����ṹ
		DBO_GR_GrowLevelParameter GrowLevelParameter;
		ZeroMemory(&GrowLevelParameter,sizeof(GrowLevelParameter));

		//ִ�гɹ�
		if(lResultCode==DB_SUCCESS)
		{
			//��������
			if (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				//��ȡ����
				GrowLevelParameter.wCurrLevelID = m_PlatformDBAide.GetValue_WORD(TEXT("CurrLevelID"));				
				GrowLevelParameter.dwExperience = m_PlatformDBAide.GetValue_DWORD(TEXT("Experience"));
				GrowLevelParameter.dwUpgradeExperience = m_PlatformDBAide.GetValue_DWORD(TEXT("UpgradeExperience"));
				GrowLevelParameter.lUpgradeRewardGold = m_PlatformDBAide.GetValue_LONGLONG(TEXT("RewardGold"));
				GrowLevelParameter.lUpgradeRewardIngot = m_PlatformDBAide.GetValue_LONGLONG(TEXT("RewardMedal"));			
			}	

			//����ṹ
			DBO_GR_GrowLevelUpgrade GrowLevelUpgrade;
			ZeroMemory(&GrowLevelUpgrade,sizeof(GrowLevelUpgrade));

			//������ʾ
			CDBVarValue DBVarValue;
			m_PlatformDBModule->GetParameter(TEXT("@strUpGradeDescribe"),DBVarValue);
			lstrcpyn(GrowLevelUpgrade.szNotifyContent,CW2CT(DBVarValue.bstrVal),CountArray(GrowLevelUpgrade.szNotifyContent));
			if(GrowLevelUpgrade.szNotifyContent[0]!=0)
			{
				//��ȡ�Ƹ�
				GrowLevelUpgrade.lCurrScore = m_PlatformDBAide.GetValue_LONGLONG(TEXT("Score"));
				GrowLevelUpgrade.lCurrIngot = m_PlatformDBAide.GetValue_LONGLONG(TEXT("Ingot"));	

				//������ʾ
				WORD wSendDataSize = sizeof(GrowLevelUpgrade)-sizeof(GrowLevelUpgrade.szNotifyContent);
				wSendDataSize += CountStringBuffer(GrowLevelUpgrade.szNotifyContent);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROWLEVEL_UPGRADE,dwContextID,&GrowLevelUpgrade,wSendDataSize);
			}
		}

		//���Ͳ���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROWLEVEL_PARAMETER,dwContextID,&GrowLevelParameter,sizeof(GrowLevelParameter));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		
		//����ṹ
		DBO_GR_GrowLevelParameter GrowLevelParameter;
		ZeroMemory(&GrowLevelParameter,sizeof(GrowLevelParameter));

		//���Ͳ���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GROWLEVEL_PARAMETER,dwContextID,&GrowLevelParameter,sizeof(GrowLevelParameter));

		return false;
	}
}

//�û�Ȩ��
bool CDataBaseEngineSink::OnRequestManageUserRight(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_ManageUserRight));
		if (wDataSize!=sizeof(DBR_GR_ManageUserRight)) return false;

		//������
		DBR_GR_ManageUserRight * pManageUserRight=(DBR_GR_ManageUserRight *)pData;
		dwUserID=pManageUserRight->dwUserID;

		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pManageUserRight->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@dwAddRight"),pManageUserRight->dwAddRight);
		m_GameDBAide.AddParameter(TEXT("@dwRemoveRight"),pManageUserRight->dwRemoveRight);

		//ִ�в�ѯ
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_ManageUserRight"),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		
		return false;
	}

	return true;
}

//����Ȩ��
bool CDataBaseEngineSink::OnRequestManageMatchRight(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_ManageUserRight));
		if (wDataSize!=sizeof(DBR_GR_ManageUserRight)) return false;

		//������
		DBR_GR_ManageMatchRight * pManageMatchRight=(DBR_GR_ManageMatchRight *)pData;
		dwUserID=pManageMatchRight->dwUserID;

		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pManageMatchRight->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@dwAddRight"),pManageMatchRight->dwAddRight);
		m_GameDBAide.AddParameter(TEXT("@dwRemoveRight"),pManageMatchRight->dwRemoveRight);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pManageMatchRight->dwMatchID);
		m_GameDBAide.AddParameter(TEXT("@dwMatchNo"),pManageMatchRight->dwMatchNO);

		//ִ�в�ѯ
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_ManageMatchRight"),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		
		//������
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false);

		return false;
	}

	return true;
}

//ϵͳ��Ϣ
bool CDataBaseEngineSink::OnRequestLoadSystemMessage(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);

		//ִ�в�ѯ
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_LoadSystemMessage"),true);

		//�������
		if(lReturnValue==0)
		{
			TCHAR szServerID[32]={0};
			_sntprintf(szServerID, CountArray(szServerID), TEXT("%d"), m_pGameServiceOption->wServerID);

			while(true)
			{
				//�����ж�
				if (m_GameDBModule->IsRecordsetEnd()==true) break;

				//�������
				TCHAR szServerRange[1024]={0};
				CString strServerRange;
				bool bSendMessage=false;
				bool bAllRoom=false;

				//��ȡ��Χ
				m_GameDBAide.GetValue_String(TEXT("ServerRange"), szServerRange, CountArray(szServerRange));
				szServerRange[1023]=0;
				strServerRange.Format(TEXT("%s"), szServerRange);

				//��Χ�ж�
				while(true)
				{
					int nfind=strServerRange.Find(TEXT(','));
					if(nfind!=-1 && nfind>0)
					{
						CString strID=strServerRange.Left(nfind);
						WORD wServerID=StrToInt(strID);
						bSendMessage=(wServerID==0 || wServerID==m_pGameServiceOption->wServerID);
						if(wServerID==0)bAllRoom=true;

						if(bSendMessage) break;

						strServerRange=strServerRange.Right(strServerRange.GetLength()-nfind-1);
					}
					else
					{
						WORD wServerID=StrToInt(szServerRange);
						bSendMessage=(wServerID==0 || wServerID==m_pGameServiceOption->wServerID);
						if(wServerID==0)bAllRoom=true;

						break;
					}
				}

				//������Ϣ
				if(bSendMessage)
				{
					//�������
					DBR_GR_SystemMessage SystemMessage;
					ZeroMemory(&SystemMessage, sizeof(SystemMessage));

					//��ȡ��Ϣ
					SystemMessage.dwMessageID=m_GameDBAide.GetValue_DWORD(TEXT("ID"));
					SystemMessage.cbMessageType=m_GameDBAide.GetValue_BYTE(TEXT("MessageType"));
					SystemMessage.dwTimeRate=m_GameDBAide.GetValue_DWORD(TEXT("TimeRate"));
					SystemMessage.cbAllRoom=bAllRoom?TRUE:FALSE;
					m_GameDBAide.GetValue_String(TEXT("MessageString"),SystemMessage.szSystemMessage,CountArray(SystemMessage.szSystemMessage));

					//��ȡʱ��
					SYSTEMTIME systime;
					ZeroMemory(&systime, sizeof(systime));

					//��ʼʱ��
					m_GameDBAide.GetValue_SystemTime(TEXT("StartTime"),systime);
					CTime StarTime(systime);
					SystemMessage.tStartTime=StarTime.GetTime();

					//����ʱ��
					m_GameDBAide.GetValue_SystemTime(TEXT("ConcludeTime"),systime);
					CTime ConcludeTime(systime);
					SystemMessage.tConcludeTime=ConcludeTime.GetTime();

					//���ͽ��
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SYSTEM_MESSAGE_RESULT,dwContextID,&SystemMessage,sizeof(SystemMessage));
				}

				//��һ��
				m_GameDBModule->MoveToNext();
			}

			//�������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SYSTEM_MESSAGE_FINISH,dwContextID,NULL,0);			
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		
		//������
		OnInsureDisposeResult(dwContextID,DB_ERROR,0L,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false);

		return false;
	}

	return true;
}


//�������д�
bool CDataBaseEngineSink::OnRequestLoadSensitiveWords(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//�������
		m_PlatformDBAide.ResetParameter();

		//ִ�в�ѯ
		LONG lReturnValue=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_LoadSensitiveWords"),true);

		//��ȡ��Ϣ
		if (lReturnValue==DB_SUCCESS)
		{
			//��ʼ��Ϣ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SENSITIVE_WORDS,0xfffe,NULL,0);

			//��ȡ��Ϣ
			while (m_PlatformDBModule->IsRecordsetEnd()==false)
			{
				//��������
				TCHAR szSensitiveWords[32]=TEXT("");

				//��ȡ��Ϣ
				m_PlatformDBAide.GetValue_String(TEXT("SensitiveWords"),szSensitiveWords,CountArray(szSensitiveWords));				

				//������Ϣ
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SENSITIVE_WORDS,0,szSensitiveWords,sizeof(szSensitiveWords));

				//�ƶ���¼
				m_PlatformDBModule->MoveToNext();
			};

			//������Ϣ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SENSITIVE_WORDS,0xffff,NULL,0);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		
		return false;
	}

	return true;
}

//����������
bool CDataBaseEngineSink::OnRequestUnlockAndroidUser(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_UnlockAndroidUser));
		if (wDataSize!=sizeof(DBR_GR_UnlockAndroidUser)) return false;

		//������
		DBR_GR_UnlockAndroidUser * pUnlockAndroidUser=(DBR_GR_UnlockAndroidUser *)pData;

		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@wServerID"),pUnlockAndroidUser->wServerID);
		m_GameDBAide.AddParameter(TEXT("@wBatchID"),pUnlockAndroidUser->wBatchID);

		//ִ�в�ѯ
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_UnlockAndroidUser"),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//��¼���
VOID CDataBaseEngineSink::OnLogonDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient,BYTE cbDeviceType,WORD wBehaviorFlags,WORD wPageTableCount)
{
	if (dwErrorCode==DB_SUCCESS)
	{
		//��������
		m_LogonSuccess.wFaceID=m_GameDBAide.GetValue_WORD(TEXT("FaceID"));
		m_LogonSuccess.dwUserID=m_GameDBAide.GetValue_DWORD(TEXT("UserID"));
		m_LogonSuccess.dwGameID=m_GameDBAide.GetValue_DWORD(TEXT("GameID"));
		m_LogonSuccess.dwGroupID=m_GameDBAide.GetValue_DWORD(TEXT("GroupID"));
		m_LogonSuccess.dwCustomID=m_GameDBAide.GetValue_DWORD(TEXT("CustomID"));
		m_GameDBAide.GetValue_String(TEXT("NickName"),m_LogonSuccess.szNickName,CountArray(m_LogonSuccess.szNickName));
		m_GameDBAide.GetValue_String(TEXT("GroupName"),m_LogonSuccess.szGroupName,CountArray(m_LogonSuccess.szGroupName));
		m_GameDBAide.GetValue_String(TEXT("UserAddress"),m_LogonSuccess.szUserAddress,CountArray(m_LogonSuccess.szUserAddress));

		//�û�����
		m_LogonSuccess.cbGender=m_GameDBAide.GetValue_BYTE(TEXT("Gender"));
		m_LogonSuccess.cbMemberOrder=m_GameDBAide.GetValue_BYTE(TEXT("MemberOrder"));
		m_LogonSuccess.cbMasterOrder=m_GameDBAide.GetValue_BYTE(TEXT("MasterOrder"));
		m_GameDBAide.GetValue_String(TEXT("UnderWrite"),m_LogonSuccess.szUnderWrite,CountArray(m_LogonSuccess.szUnderWrite));

		//������Ϣ
		m_LogonSuccess.lScore=m_GameDBAide.GetValue_LONGLONG(TEXT("Score"));
		m_LogonSuccess.lIngot=m_GameDBAide.GetValue_LONG(TEXT("Ingot"));
		m_LogonSuccess.lGrade=m_GameDBAide.GetValue_LONGLONG(TEXT("Grade"));
		m_LogonSuccess.lInsure=m_GameDBAide.GetValue_LONGLONG(TEXT("Insure"));
		m_LogonSuccess.dBeans=m_GameDBAide.GetValue_DOUBLE(TEXT("Beans"));

		//������Ϣ
		m_LogonSuccess.dwWinCount=m_GameDBAide.GetValue_LONG(TEXT("WinCount"));
		m_LogonSuccess.dwLostCount=m_GameDBAide.GetValue_LONG(TEXT("LostCount"));
		m_LogonSuccess.dwDrawCount=m_GameDBAide.GetValue_LONG(TEXT("DrawCount"));
		m_LogonSuccess.dwFleeCount=m_GameDBAide.GetValue_LONG(TEXT("FleeCount"));		
		m_LogonSuccess.dwExperience=m_GameDBAide.GetValue_LONG(TEXT("Experience"));
		m_LogonSuccess.lLoveLiness=m_GameDBAide.GetValue_LONG(TEXT("LoveLiness"));

		//������Ϣ		
		m_LogonSuccess.dwUserRight=m_GameDBAide.GetValue_DWORD(TEXT("UserRight"));
		m_LogonSuccess.dwMasterRight=m_GameDBAide.GetValue_DWORD(TEXT("MasterRight"));
		m_LogonSuccess.cbDeviceType=cbDeviceType;
		m_LogonSuccess.wBehaviorFlags=wBehaviorFlags;
		m_LogonSuccess.wPageTableCount=wPageTableCount;

		//��������
		m_LogonSuccess.dwInoutIndex=m_GameDBAide.GetValue_DWORD(TEXT("InoutIndex"));

		//��ȡ��Ϣ
		if(pszErrorString!=NULL) lstrcpyn(m_LogonSuccess.szDescribeString,pszErrorString,CountArray(m_LogonSuccess.szDescribeString));
		
		//�������
		m_LogonSuccess.wTaskCount=0;
		ZeroMemory(m_LogonSuccess.UserTaskInfo,sizeof(m_LogonSuccess.UserTaskInfo));

		try
		{
			//�������
			m_PlatformDBAide.ResetParameter();
			m_PlatformDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
			m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),m_LogonSuccess.dwUserID);
			m_PlatformDBAide.AddParameter(TEXT("@strPassword"),m_LogonSuccess.szPassword);

			//�������
			TCHAR szDescribeString[128]=TEXT("");
			m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

			//ִ�нű�
			LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_QueryTaskInfo"),true);

			//ִ�гɹ�
			if(lResultCode==DB_SUCCESS)
			{
				//��������
				tagUserTaskInfo * pUserTaskInfo=NULL;

				//ѭ����ȡ
				while (m_PlatformDBModule->IsRecordsetEnd()==false)
				{
					//���ñ���
					pUserTaskInfo = &m_LogonSuccess.UserTaskInfo[m_LogonSuccess.wTaskCount++];

					//��ȡ����
					pUserTaskInfo->wTaskID = m_PlatformDBAide.GetValue_WORD(TEXT("TaskID"));
					pUserTaskInfo->cbTaskStatus = m_PlatformDBAide.GetValue_BYTE(TEXT("TaskStatus"));
					pUserTaskInfo->wTaskProgress = m_PlatformDBAide.GetValue_WORD(TEXT("Progress"));
					pUserTaskInfo->dwResidueTime = m_PlatformDBAide.GetValue_DWORD(TEXT("ResidueTime"));
					pUserTaskInfo->dwLastUpdateTime=(DWORD)time(NULL);

					//�ƶ���¼
					m_PlatformDBModule->MoveToNext();
				}
			}
		}catch(...)
		{
			ASSERT(FALSE);
		}

		try
		{
			//�������
			m_TreasureDBAide.ResetParameter();
			m_TreasureDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
			m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),m_LogonSuccess.dwUserID);
			m_TreasureDBAide.AddParameter(TEXT("@strPassword"),m_LogonSuccess.szPassword);

			//�������
			TCHAR szDescribeString[128]=TEXT("");
			m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

			//ִ�нű�
			LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GR_QueryUserGameData"),true);

			//ִ�гɹ�
			if(lResultCode==DB_SUCCESS)
			{
				//��ȡ����
				m_TreasureDBAide.GetValue_String(TEXT("UserGameData"),m_LogonSuccess.szUserGameData,CountArray(m_LogonSuccess.szUserGameData));
			}
		}catch(...)
		{
			ASSERT(FALSE);
		}

		//���ͽ��
		WORD wDataSize=sizeof(m_LogonSuccess.UserTaskInfo[0])*m_LogonSuccess.wTaskCount;
		WORD wHeadSize=sizeof(m_LogonSuccess)-sizeof(m_LogonSuccess.UserTaskInfo);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_SUCCESS,dwContextID,&m_LogonSuccess,wHeadSize+wDataSize);
	}
	else
	{
		//��������
		DBO_GR_LogonFailure LogonFailure;
		ZeroMemory(&LogonFailure,sizeof(LogonFailure));

		//��������
		LogonFailure.lResultCode=dwErrorCode;
		lstrcpyn(LogonFailure.szDescribeString,pszErrorString,CountArray(LogonFailure.szDescribeString));

		//���ͽ��
		WORD wDataSize=CountStringBuffer(LogonFailure.szDescribeString);
		WORD wHeadSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_FAILURE,dwContextID,&LogonFailure,wHeadSize+wDataSize);
	}

	return;
}

//���н��
VOID CDataBaseEngineSink::OnInsureDisposeResult(DWORD dwContextID, DWORD dwErrorCode, SCORE lFrozenedScore, LPCTSTR pszErrorString, bool bMobileClient,BYTE cbActivityGame)
{
	if (dwErrorCode==DB_SUCCESS)
	{
		//��������
		DBO_GR_UserInsureSuccess UserInsureSuccess;
		ZeroMemory(&UserInsureSuccess,sizeof(UserInsureSuccess));

		//�������
		UserInsureSuccess.cbActivityGame=cbActivityGame;
		UserInsureSuccess.lFrozenedScore=lFrozenedScore;
		UserInsureSuccess.dwUserID=m_TreasureDBAide.GetValue_DWORD(TEXT("UserID"));
		UserInsureSuccess.lSourceScore=m_TreasureDBAide.GetValue_LONGLONG(TEXT("SourceScore"));
		UserInsureSuccess.lSourceInsure=m_TreasureDBAide.GetValue_LONGLONG(TEXT("SourceInsure"));
		UserInsureSuccess.lInsureRevenue=m_TreasureDBAide.GetValue_LONGLONG(TEXT("InsureRevenue"));
		UserInsureSuccess.lVariationScore=m_TreasureDBAide.GetValue_LONGLONG(TEXT("VariationScore"));
		UserInsureSuccess.lVariationInsure=m_TreasureDBAide.GetValue_LONGLONG(TEXT("VariationInsure"));
		lstrcpyn(UserInsureSuccess.szDescribeString,pszErrorString,CountArray(UserInsureSuccess.szDescribeString));

		//���ͽ��
		WORD wDataSize=CountStringBuffer(UserInsureSuccess.szDescribeString);
		WORD wHeadSize=sizeof(UserInsureSuccess)-sizeof(UserInsureSuccess.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_INSURE_SUCCESS,dwContextID,&UserInsureSuccess,wHeadSize+wDataSize);
	}
	else
	{
		//��������
		DBO_GR_UserInsureFailure UserInsureFailure;
		ZeroMemory(&UserInsureFailure,sizeof(UserInsureFailure));

		//�������
		UserInsureFailure.cbActivityGame=cbActivityGame;
		UserInsureFailure.lResultCode=dwErrorCode;
		UserInsureFailure.lFrozenedScore=lFrozenedScore;
		lstrcpyn(UserInsureFailure.szDescribeString,pszErrorString,CountArray(UserInsureFailure.szDescribeString));

		//���ͽ��
		WORD wDataSize=CountStringBuffer(UserInsureFailure.szDescribeString);
		WORD wHeadSize=sizeof(UserInsureFailure)-sizeof(UserInsureFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_INSURE_FAILURE,dwContextID,&UserInsureFailure,wHeadSize+wDataSize);
	}

	return;
}


//////////////////////////////////////////////////////////////////////////////////