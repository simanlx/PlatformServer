#ifndef DEFINE_HEAD_FILE
#define DEFINE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//��ֵ����

//ͷ���С
#define FACE_CX						48									//ͷ�����
#define FACE_CY						48									//ͷ��߶�

//���ȶ���
#define LEN_LESS_ACCOUNTS			6									//����ʺ�
#define LEN_LESS_NICKNAME			6									//����ǳ�
#define LEN_LESS_PASSWORD			6									//�������
#define LEN_LESS_COMPELLATION		4									//��С����

//��������
#define MAX_CHAIR					100									//�������
#define MAX_TABLE					512									//�������
#define MAX_COLUMN					32									//����б�
#define MAX_BATCH					48									//�������
#define MAX_ANDROID					2048									//������
#define MAX_PROPERTY				128									//������
#define MAX_WHISPER_USER			16									//���˽��

//�б�����
#define MAX_KIND					256									//�������
#define MAX_SERVER					1024								//��󷿼�

//��������
#define INVALID_CHAIR				0xFFFF								//��Ч����
#define INVALID_TABLE				0xFFFF								//��Ч����

//˰�ն���
#define REVENUE_BENCHMARK			0L								    //˰�����
#define REVENUE_DENOMINATOR			1000L								//˰�շ�ĸ

//////////////////////////////////////////////////////////////////////////////////
//ϵͳ����

//��������
#define SCORE						LONGLONG							//��������
#define SCORE_STRING				TEXT("%I64d")						//��������
#define SCORE_STRING_EX				TEXT("%s")							//��������

//��Ϸ״̬
#define GAME_STATUS_FREE			0									//����״̬
#define GAME_STATUS_PLAY			100									//��Ϸ״̬
#define GAME_STATUS_WAIT			200									//�ȴ�״̬

//ϵͳ����
#define LEN_USER_CHAT				128									//���쳤��
#define TIME_USER_CHAT				1L									//������
#define TRUMPET_MAX_CHAR            128									//���ȳ���

//////////////////////////////////////////////////////////////////////////////////
//��������

//�б�����
#define PRIME_TYPE					11L									//������Ŀ
#define PRIME_KIND					53L									//������Ŀ
#define PRIME_NODE					101L								//�ڵ���Ŀ
#define PRIME_PAGE					53L									//�Զ���Ŀ
#define PRIME_SERVER				1009L								//��������

//��������
#define PRIME_SERVER_USER			503L								//��������
#define PRIME_ANDROID_USER			503L								//��������
#define PRIME_PLATFORM_USER			100003L								//ƽ̨����

//��������
#define PRIME_ANDROID_PARAMETER		65									//������Ŀ

//////////////////////////////////////////////////////////////////////////////////
//���ݳ���

//��������
#define LEN_MD5						33									//��������
#define LEN_USERNOTE				32									//��ע����
#define LEN_ACCOUNTS				32									//�ʺų���
#define LEN_NICKNAME				32									//�ǳƳ���
#define LEN_PASSWORD				33									//���볤��
#define LEN_GROUP_NAME				32									//��������
#define LEN_UNDER_WRITE				32									//����ǩ��
#define LEN_USER_ADDR				32									//��ַ����

//���ݳ���
#define LEN_QQ						16									//Q Q ����
#define LEN_EMAIL					33									//�����ʼ�
#define LEN_USER_NOTE				256									//�û���ע
#define LEN_SEAT_PHONE				33									//�̶��绰
#define LEN_MOBILE_PHONE			12									//�ƶ��绰
#define LEN_PASS_PORT_ID			19									//֤������
#define LEN_QQ_NUMBER_ID			13									//֤������
#define LEN_COMPELLATION			16									//��ʵ����
#define LEN_DWELLING_PLACE			128									//��ϵ��ַ
#define LEN_USER_UIN				33									//UIN����
#define LEN_WEEK					7									//���ڳ���
#define LEN_TASK_NAME				64									//��������
#define LEN_TRANS_REMARK			32									//ת�˱�ע	
#define LEN_VERIFY_CODE				5									//��֤����

//������ʶ
#define LEN_NETWORK_ID				13									//��������
#define LEN_MACHINE_ID				33									//���г���

//�б�����
#define LEN_TYPE					32									//���೤��
#define LEN_KIND					32									//���ͳ���
#define LEN_NODE					32									//�ڵ㳤��
#define LEN_PAGE					32									//���Ƴ���
#define LEN_SERVER					32									//���䳤��
#define LEN_PROCESS					32									//���̳���

//////////////////////////////////////////////////////////////////////////////////

//�û���ϵ
#define	CP_NORMAL					0									//δ֪��ϵ
#define	CP_FRIEND					1									//���ѹ�ϵ
#define	CP_DETEST					2									//����ϵ
#define CP_SHIELD					3									//��������

//////////////////////////////////////////////////////////////////////////////////

//�Ա���
#define GENDER_FEMALE				0									//Ů���Ա�
#define GENDER_MANKIND				1									//�����Ա�

//////////////////////////////////////////////////////////////////////////////////

//��Ϸģʽ
#define GAME_GENRE_GOLD				0x0001								//�������
#define GAME_GENRE_SCORE			0x0002								//��ֵ����
#define GAME_GENRE_MATCH			0x0004								//��������
#define GAME_GENRE_EDUCATE			0x0008								//ѵ������
#define GAME_GENRE_MONEY			0x000F								//��Ǯ����

//��������
#define SERVER_GENRE_NORMAL         0x0001                              //��ͨ����
#define SERVER_GENRE_PASSWD         0x0002                              //��������

//����ģʽ
#define SCORE_GENRE_NORMAL			0x0100								//��ͨģʽ
#define SCORE_GENRE_POSITIVE		0x0200								//�Ǹ�ģʽ

//////////////////////////////////////////////////////////////////////////////////

//��������
#define TASK_TYPE_WIN_INNINGS		0x01								//Ӯ������
#define TASK_TYPE_SUM_INNINGS		0x02								//�ܾ�����
#define TASK_TYPE_FIRST_WIN			0x04								//��ʤ����
#define TASK_TYPE_JOIN_MATCH		0x08								//��������

//����״̬
#define TASK_STATUS_UNFINISH		0									//����״̬
#define TASK_STATUS_SUCCESS			1									//����ɹ�
#define TASK_STATUS_FAILED			2									//����ʧ��	
#define TASK_STATUS_REWARD			3									//��ȡ����

//��������
#define TASK_MAX_COUNT				128									//��������

//////////////////////////////////////////////////////////////////////////////////
//�齱����
#define MAX_LOTTERY					16									//��ཱ��

//��Ϸ����
#define LEN_GAME_DATA				1024								//��Ϸ����
#define LEN_GAME_ITEM				64									//��������
#define MAX_DATA_ITEM				16									//�������

//////////////////////////////////////////////////////////////////////////////////

//�û�״̬
#define US_NULL						0x00								//û��״̬
#define US_FREE						0x01								//վ��״̬
#define US_SIT						0x02								//����״̬
#define US_READY					0x03								//ͬ��״̬
#define US_LOOKON					0x04								//�Թ�״̬
#define US_PLAYING					0x05								//��Ϸ״̬
#define US_OFFLINE					0x06								//����״̬

//////////////////////////////////////////////////////////////////////////////////

//����״̬
#define MS_NULL						0x00								//û��״̬
#define MS_SIGNUP					0x01								//����״̬
#define MS_MATCHING					0x02								//��������
#define MS_MATCHEND					0x03								//��������

//��Ҳ���״̬
#define MUS_NULL					0x00								//δ����
#define MUS_SIGNUP					0x01								//�ѱ���
#define MUS_MATCHING				0x02								//������

//////////////////////////////////////////////////////////////////////////////////

//�������
#define SRL_LOOKON					0x00000001							//�Թ۱�־
#define SRL_OFFLINE					0x00000002							//���߱�־
#define SRL_SAME_IP					0x00000004							//ͬ����־

//�������
#define SRL_ROOM_CHAT				0x00000100							//�����־
#define SRL_GAME_CHAT				0x00000200							//�����־
#define SRL_WISPER_CHAT				0x00000400							//˽�ı�־
#define SRL_HIDE_USER_INFO			0x00000800							//���ر�־

//////////////////////////////////////////////////////////////////////////////////
//�б�����

//��Ч����
#define UD_NULL						0									//��Ч����
#define UD_IMAGE					100									//ͼ������
#define UD_CUSTOM					200									//�Զ�����

//��������
#define UD_GAME_ID					1									//��Ϸ��ʶ
#define UD_USER_ID					2									//�û���ʶ
#define	UD_NICKNAME					3									//�û��ǳ�

//��չ����
#define UD_GENDER					10									//�û��Ա�
#define UD_GROUP_NAME				11									//��������
#define UD_UNDER_WRITE				12									//����ǩ��

//״̬��Ϣ
#define UD_TABLE					20									//��Ϸ����
#define UD_CHAIR					21									//���Ӻ���

//������Ϣ
#define UD_SCORE					30									//�û�����
#define UD_GRADE					31									//�û��ɼ�
#define UD_USER_MEDAL				32									//�û�����
#define UD_EXPERIENCE				33									//�û�����
#define UD_LOVELINESS				34									//�û�����
#define UD_WIN_COUNT				35									//ʤ������
#define UD_LOST_COUNT				36									//�������
#define UD_DRAW_COUNT				37									//�;�����
#define UD_FLEE_COUNT				38									//�Ӿ�����
#define UD_PLAY_COUNT				39									//�ܾ�����

//���ֱ���
#define UD_WIN_RATE					40									//�û�ʤ��
#define UD_LOST_RATE				41									//�û�����
#define UD_DRAW_RATE				42									//�û�����
#define UD_FLEE_RATE				43									//�û�����
#define UD_GAME_LEVEL				44									//��Ϸ�ȼ�

//��չ��Ϣ
#define UD_NOTE_INFO				50									//�û���ע
#define UD_LOOKON_USER				51									//�Թ��û�

//ͼ���б�
#define UD_IMAGE_FLAG				(UD_IMAGE+1)						//�û���־
#define UD_IMAGE_GENDER				(UD_IMAGE+2)						//�û��Ա�
#define UD_IMAGE_STATUS				(UD_IMAGE+3)						//�û�״̬

//////////////////////////////////////////////////////////////////////////////////
//���ݿⶨ��

#define DB_ERROR 					-1  								//����ʧ��
#define DB_SUCCESS 					0  									//�����ɹ�
#define DB_NEEDMB 					18 									//����ʧ��
#define DB_PASSPORT					19									//����ʧ��

//////////////////////////////////////////////////////////////////////////////////
//���߱�ʾ
#define PT_USE_MARK_DOUBLE_SCORE    0x0001								//˫������
#define PT_USE_MARK_FOURE_SCORE     0x0002								//�ı�����
#define PT_USE_MARK_GUARDKICK_CARD  0x0010								//���ߵ���
#define PT_USE_MARK_POSSESS         0x0020								//�������� 

#define MAX_PT_MARK                 4                                   //��ʶ��Ŀ

//��Ч��Χ
#define VALID_TIME_DOUBLE_SCORE     3600                                //��Чʱ��
#define VALID_TIME_FOUR_SCORE       3600                                //��Чʱ��
#define VALID_TIME_GUARDKICK_CARD   3600                                //����ʱ��
#define VALID_TIME_POSSESS          3600                                //����ʱ��
#define VALID_TIME_KICK_BY_MANAGER  3600                                //��Ϸʱ�� 

//////////////////////////////////////////////////////////////////////////////////
//�豸����
#define DEVICE_TYPE_PC              0x00                                //PC
#define DEVICE_TYPE_ANDROID         0x10                                //Android
#define DEVICE_TYPE_ITOUCH          0x20                                //iTouch
#define DEVICE_TYPE_IPHONE          0x40                                //iPhone
#define DEVICE_TYPE_IPAD            0x80                                //iPad

/////////////////////////////////////////////////////////////////////////////////
//�ֻ�����

//��ͼģʽ
#define	VIEW_MODE_ALL				0x0001								//ȫ������
#define	VIEW_MODE_PART				0x0002								//���ֿ���

//��Ϣģʽ
#define VIEW_INFO_LEVEL_1			0x0010								//������Ϣ
#define VIEW_INFO_LEVEL_2			0x0020								//������Ϣ
#define VIEW_INFO_LEVEL_3			0x0040								//������Ϣ
#define VIEW_INFO_LEVEL_4			0x0080								//������Ϣ

//��������
#define RECVICE_GAME_CHAT			0x0100								//��������
#define RECVICE_ROOM_CHAT			0x0200								//��������
#define RECVICE_ROOM_WHISPER		0x0400								//����˽��

//��Ϊ��ʶ
#define BEHAVIOR_LOGON_NORMAL       0x0000                              //��ͨ��¼
#define BEHAVIOR_LOGON_IMMEDIATELY  0x1000                              //������¼

/////////////////////////////////////////////////////////////////////////////////
//�������
#define RESULT_ERROR 					-1  								//��������
#define RESULT_SUCCESS 					0  									//�����ɹ�
#define RESULT_FAIL 					1  									//����ʧ��

/////////////////////////////////////////////////////////////////////////////////
//�仯ԭ��
#define SCORE_REASON_WRITE              0                                   //д�ֱ仯
#define SCORE_REASON_INSURE             1                                   //���б仯
#define SCORE_REASON_PROPERTY           2                                   //���߱仯
#define SCORE_REASON_MATCH_FEE          3                                   //��������
#define SCORE_REASON_MATCH_QUIT         4                                   //��������

/////////////////////////////////////////////////////////////////////////////////

//��¼����ʧ��ԭ��
#define LOGON_FAIL_SERVER_INVALIDATION  200   

/////////////////////////////////////////////////////////////////////////////////

//��������
#define ANDROID_SIMULATE				0x01								//�໥ģ��
#define ANDROID_PASSIVITY				0x02								//�������
#define ANDROID_INITIATIVE				0x04								//�������

/////////////////////////////////////////////////////////////////////////////////
//��������

//�۷�����
#define MATCH_FEE_TYPE_GOLD				0x00								//�۷�����
#define MATCH_FEE_TYPE_MEDAL			0x01								//�۷�����

//��������
#define MATCH_TYPE_LOCKTIME				0x00								//��ʱ����
#define MATCH_TYPE_IMMEDIATE			0x01								//��ʱ����

////////////////////////////////////////////////////////////////////////////////
//����ģʽ

#define LAYOUT_MODE_TREE				0									//��״����
#define LAYOUT_MODE_BULK				1									//��״����

////////////////////////////////////////////////////////////////////////////////

#endif