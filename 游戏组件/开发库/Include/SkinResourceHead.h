#ifndef SKIN_RESOURCE_HEAD_HEAD_FILE
#define SKIN_RESOURCE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//��Ϸ��Դ

//��Ϸ����
struct tagGameBarImage
{
	//������Դ
	UINT							uGameBar;							//��Դ��ʶ

	//���ư�ť
	UINT							uBtHideControl;						//��Դ��ʶ
	UINT							uBtShowControl;						//��Դ��ʶ

	//���ܰ�ť
	UINT							uBtMore;							//��Դ��ʶ
	UINT							uBtInvite;							//��Դ��ʶ
	UINT							uBtOption;							//��Դ��ʶ
	UINT							uBtLimitSound;						//��Դ��ʶ
	UINT							uBtAllowSound;						//��Դ��ʶ
};

//��Ϸ���
struct tagGameBillImage
{
	//�����Դ
	UINT							uBillL;								//��Դ��ʶ
	UINT							uBillM;								//��Դ��ʶ
	UINT							uBillR;								//��Դ��ʶ

	//��水ť
	UINT							uBtShowBill;						//��Դ��ʶ
	UINT							uBtHideBill;						//��Դ��ʶ
};

//��Ϸ����
struct tagGameChatImage
{
	//������Դ
	UINT							uChatML;							//��Դ��ʶ
	UINT							uChatMR;							//��Դ��ʶ
	UINT							uChatTL;							//��Դ��ʶ
	UINT							uChatTM;							//��Դ��ʶ
	UINT							uChatTR;							//��Դ��ʶ
	UINT							uChatBL;							//��Դ��ʶ
	UINT							uChatBM;							//��Դ��ʶ
	UINT							uChatBR;							//��Դ��ʶ

	//������Դ
	UINT							uChatHML;							//��Դ��ʶ
	UINT							uChatHMR;							//��Դ��ʶ
	UINT							uChatHTL;							//��Դ��ʶ
	UINT							uChatHTM;							//��Դ��ʶ
	UINT							uChatHTR;							//��Դ��ʶ
	UINT							uChatHBL;							//��Դ��ʶ
	UINT							uChatHBM;							//��Դ��ʶ
	UINT							uChatHBR;							//��Դ��ʶ

	//���찴ť
	UINT							uBtPhrase;							//��Դ��ʶ
	UINT							uBtStopChat;						//��Դ��ʶ
	UINT							uBtRecvChat;						//��Դ��ʶ
	UINT							uBtColorSet;						//��Դ��ʶ
	UINT							uBtSaveChat;						//��Դ��ʶ
	UINT							uBtSendChat;						//��Դ��ʶ
	UINT							uBtChatShort;						//��Դ��ʶ
	UINT							uBtExpression;						//��Դ��ʶ
	UINT							uBtCleanScreen;						//��Դ��ʶ
	UINT							uBtMoreFunction;					//��Դ��ʶ
	UINT							uBtBugle;							//��Դ��ʶ

	//�������
	UINT							uBtChatClose;						//��Դ��ʶ
	UINT							uBtChatControl;						//��Դ��ʶ
	UINT							uChatControlBack;					//��Դ��ʶ

	//��ɫ����
	COLORREF						crChatBK;							//��ɫ����
};

//��Ϸ���
struct tagGameFrameImage
{
	//��ɫ����
	COLORREF						crTitleTX;							//��ɫ����

	//�����Դ
	UINT							uFrameML;							//��Դ��ʶ
	UINT							uFrameMR;							//��Դ��ʶ
	UINT							uFrameTL;							//��Դ��ʶ
	UINT							uFrameTM;							//��Դ��ʶ
	UINT							uFrameTR;							//��Դ��ʶ
	UINT							uFrameBL;							//��Դ��ʶ
	UINT							uFrameBM;							//��Դ��ʶ
	UINT							uFrameBR;							//��Դ��ʶ

	//��ܰ�ť
	UINT							uBtFrameMin;						//��Դ��ʶ
	UINT							uBtFrameMax;						//��Դ��ʶ
	UINT							uBtFrameClose;						//��Դ��ʶ
	UINT							uBtFrameResore;						//��Դ��ʶ
};

//��Ϸ�û�
struct tagGameUserInfoImage
{
	//�û���Դ
	UINT							uUserInfoML;						//��Դ��ʶ
	UINT							uUserInfoMR;						//��Դ��ʶ
	UINT							uUserInfoTL;						//��Դ��ʶ
	UINT							uUserInfoTM;						//��Դ��ʶ
	UINT							uUserInfoTR;						//��Դ��ʶ
	UINT							uUserInfoBL;						//��Դ��ʶ
	UINT							uUserInfoBM;						//��Դ��ʶ
	UINT							uUserInfoBR;						//��Դ��ʶ
};

//��Ϸ�б�
struct tagGameUserListImage
{
	//�û���Դ
	UINT							uUserListML;						//��Դ��ʶ
	UINT							uUserListMR;						//��Դ��ʶ
	UINT							uUserListTL;						//��Դ��ʶ
	UINT							uUserListTM;						//��Դ��ʶ
	UINT							uUserListTR;						//��Դ��ʶ
	UINT							uUserListBL;						//��Դ��ʶ
	UINT							uUserListBM;						//��Դ��ʶ
	UINT							uUserListBR;						//��Դ��ʶ

	//�û���Դ
	UINT							uUserListHML;						//��Դ��ʶ
	UINT							uUserListHMR;						//��Դ��ʶ
	UINT							uUserListHTL;						//��Դ��ʶ
	UINT							uUserListHTM;						//��Դ��ʶ
	UINT							uUserListHTR;						//��Դ��ʶ
	UINT							uUserListHBL;						//��Դ��ʶ
	UINT							uUserListHBM;						//��Դ��ʶ
	UINT							uUserListHBR;						//��Դ��ʶ

	//��ɫ����
	COLORREF						crUserListSelectTX;					//������ɫ
	COLORREF						crUserListSelectBK;					//������ɫ
	COLORREF						crUserListNormalTX;					//������ɫ
	COLORREF						crUserListNormalBK;					//������ɫ

	//��ɫ����
	COLORREF						crUserListMyselfTX;					//������ɫ
	COLORREF						crUserListMyselfBK;					//������ɫ
	COLORREF						crUserListMasterTX;					//������ɫ
	COLORREF						crUserListMasterBK;					//������ɫ
	COLORREF						crUserListMemberTX;					//������ɫ
	COLORREF						crUserListMemberBK;					//������ɫ
};

//////////////////////////////////////////////////////////////////////////
//������Դ

//�������
struct tagPlazaFrameImage
{
	//��ɫ����
	COLORREF						crSplitter;							//��ɫ����

	//�����Դ
	UINT							uFrameML;							//��Դ��ʶ
	UINT							uFrameMR;							//��Դ��ʶ
	UINT							uFrameTL;							//��Դ��ʶ
	UINT							uFrameTM;							//��Դ��ʶ
	UINT							uFrameTR;							//��Դ��ʶ
	UINT							uFrameBL;							//��Դ��ʶ
	UINT							uFrameBM;							//��Դ��ʶ
	UINT							uFrameBR;							//��Դ��ʶ

	//�����Դ
	UINT							uItemFrameML;						//��Դ��ʶ
	UINT							uItemFrameMR;						//��Դ��ʶ
	UINT							uItemFrameTL;						//��Դ��ʶ
	UINT							uItemFrameTM;						//��Դ��ʶ
	UINT							uItemFrameTR;						//��Դ��ʶ
	UINT							uItemFrameBL;						//��Դ��ʶ
	UINT							uItemFrameBM;						//��Դ��ʶ
	UINT							uItemFrameBR;						//��Դ��ʶ

	//��ܰ�ť
	UINT							uBtFrameMin;						//��Դ��ʶ
	UINT							uBtFrameMax;						//��Դ��ʶ
	UINT							uBtFrameClose;						//��Դ��ʶ
	UINT							uBtFrameResore;						//��Դ��ʶ

	//��ܰ�ť
	UINT							uBtFrameOption;						//��Դ��ʶ
	UINT							uBtFrameCompanion;					//��Դ��ʶ
	UINT							uBtFrameBugReport;					//��Դ��ʶ
	UINT							uBtFrameSystemAffiche;				//��Դ��ʶ
	UINT							uBtFrameBank;						//��Դ��ʶ
	UINT							uBtFrameBaseEnsure;					//��Դ��ʶ

	//�л���ť
	UINT							uBtShowGameList;					//��Դ��ʶ
	UINT							uBtHideGameList;					//��Դ��ʶ
	UINT							uBtFrameItemNormal;					//��Դ��ʶ
	UINT							uBtFrameItemActive;					//��Դ��ʶ

	//������ť
	UINT							uFrameNavigation[5];				//��Դ��ʶ
};

//�����б�
struct tagPlazaServerImage
{
	//�б���Դ
	UINT							uServerML;							//��Դ��ʶ
	UINT							uServerMR;							//��Դ��ʶ
	UINT							uServerTL;							//��Դ��ʶ
	UINT							uServerTM;							//��Դ��ʶ
	UINT							uServerTR;							//��Դ��ʶ
	UINT							uServerBL;							//��Դ��ʶ
	UINT							uServerBM;							//��Դ��ʶ
	UINT							uServerBR;							//��Դ��ʶ

	//��ɫ����
	COLORREF						crServerTX;							//��ɫ����
	COLORREF						crServerBK;							//��ɫ����
};

//�㳡���
struct tagPlazaBrowserImage
{
	//��ť��Դ
	UINT							uBtLast;							//��Դ��ʶ
	UINT							uBtNext;							//��Դ��ʶ
	UINT							uBtStop;							//��Դ��ʶ
	UINT							uBtHome;							//��Դ��ʶ
	UINT							uBtReload;							//��Դ��ʶ

	//�����Դ
	UINT							uBrowserML;							//��Դ��ʶ
	UINT							uBrowserMR;							//��Դ��ʶ
	UINT							uBrowserTL;							//��Դ��ʶ
	UINT							uBrowserTM;							//��Դ��ʶ
	UINT							uBrowserTR;							//��Դ��ʶ
	UINT							uBrowserBL;							//��Դ��ʶ
	UINT							uBrowserBM;							//��Դ��ʶ
	UINT							uBrowserBR;							//��Դ��ʶ
};

//��������
struct tagPlazaTableImage
{
	//������Դ
	UINT							uTableML;							//��Դ��ʶ
	UINT							uTableMR;							//��Դ��ʶ
	UINT							uTableTL;							//��Դ��ʶ
	UINT							uTableTM;							//��Դ��ʶ
	UINT							uTableTR;							//��Դ��ʶ
	UINT							uTableBL;							//��Դ��ʶ
	UINT							uTableBM;							//��Դ��ʶ
	UINT							uTableBR;							//��Դ��ʶ

	//������Դ
	UINT							uTableNumber;						//��Դ��ʶ
	UINT							uTableGround;						//��Դ��ʶ
	UINT							uTableInvalid;						//��Դ��ʶ

	//���Ӱ�ť
	UINT							uBtAutoJoin;						//��Դ��ʶ
	UINT							uBtJoinMenu;						//��Դ��ʶ
	UINT							uBtViewMode;						//��Դ��ʶ
	UINT							uBtQuitGame;						//��Դ��ʶ
	UINT							uBtCallMaster;						//��Դ��ʶ
	UINT							uBtSearchUser;						//��Դ��ʶ
	UINT							uBtSecretTable;						//��Դ��ʶ
	UINT							uBtUnSecretTable;					//��Դ��ʶ
	UINT							uBtServerManage;					//��Դ��ʶ
};

//��������
struct tagPlazaChatImage
{
	//��ɫ����
	COLORREF						crChatBK;							//��ɫ����
	COLORREF						crSplitter;							//��ɫ����

	//������Դ
	UINT							uChatML;							//��Դ��ʶ
	UINT							uChatMR;							//��Դ��ʶ
	UINT							uChatTL;							//��Դ��ʶ
	UINT							uChatTM;							//��Դ��ʶ
	UINT							uChatTR;							//��Դ��ʶ
	UINT							uChatBL;							//��Դ��ʶ
	UINT							uChatBM;							//��Դ��ʶ
	UINT							uChatBR;							//��Դ��ʶ

	//���찴ť
	UINT							uBtPhrase;							//��Դ��ʶ
	UINT							uBtStopChat;						//��Դ��ʶ
	UINT							uBtRecvChat;						//��Դ��ʶ
	UINT							uBtColorSet;						//��Դ��ʶ
	UINT							uBtSaveChat;						//��Դ��ʶ
	UINT							uBtSendChat;						//��Դ��ʶ
	UINT							uBtChatShort;						//��Դ��ʶ
	UINT							uBtExpression;						//��Դ��ʶ
	UINT							uBtCleanScreen;						//��Դ��ʶ
	UINT							uBtMoreFunction;					//��Դ��ʶ
	UINT							uBtBugle;							//��Դ��ʶ

	//�������
	UINT							uBtChatClose;						//��Դ��ʶ
	UINT							uBtChatControl;						//��Դ��ʶ
	UINT							uChatControlBack;					//��Դ��ʶ
};

//�����û�
struct tagPlazaUserListImage
{
	//�б���Դ
	UINT							uUserListML;						//��Դ��ʶ
	UINT							uUserListMR;						//��Դ��ʶ
	UINT							uUserListTL;						//��Դ��ʶ
	UINT							uUserListTM;						//��Դ��ʶ
	UINT							uUserListTR;						//��Դ��ʶ
	UINT							uUserListBL;						//��Դ��ʶ
	UINT							uUserListBM;						//��Դ��ʶ
	UINT							uUserListBR;						//��Դ��ʶ

	//�л���ť
	UINT							uBtShowUserList;					//��Դ��ʶ
	UINT							uBtHideUserList;					//��Դ��ʶ
	UINT							uBtUserCompetition;					//��Դ��ʶ
	UINT							uBtNormalUserNormal;				//��Դ��ʶ
	UINT							uBtNormalUserActive;				//��Դ��ʶ
	UINT							uBtCustomUserNormal;				//��Դ��ʶ
	UINT							uBtCustomUserActive;				//��Դ��ʶ

	//��ɫ����
	COLORREF						crUserListSelectTX;					//������ɫ
	COLORREF						crUserListSelectBK;					//������ɫ
	COLORREF						crUserListNormalTX;					//������ɫ
	COLORREF						crUserListNormalBK;					//������ɫ

	//��ɫ����
	COLORREF						crUserListMyselfTX;					//������ɫ
	COLORREF						crUserListMyselfBK;					//������ɫ
	COLORREF						crUserListMasterTX;					//������ɫ
	COLORREF						crUserListMasterBK;					//������ɫ
	COLORREF						crUserListMemberTX;					//������ɫ
	COLORREF						crUserListMemberBK;					//������ɫ
};

//����˽��
struct tagPlazaWhisperImage
{
	//������Դ
	UINT							uChatML;							//��Դ��ʶ
	UINT							uChatMR;							//��Դ��ʶ
	UINT							uChatTL;							//��Դ��ʶ
	UINT							uChatTM;							//��Դ��ʶ
	UINT							uChatTR;							//��Դ��ʶ
	UINT							uChatBL;							//��Դ��ʶ
	UINT							uChatBM;							//��Դ��ʶ
	UINT							uChatBR;							//��Դ��ʶ

	//�û���Դ
	UINT							uUserML;							//��Դ��ʶ
	UINT							uUserMR;							//��Դ��ʶ
	UINT							uUserTL;							//��Դ��ʶ
	UINT							uUserTM;							//��Դ��ʶ
	UINT							uUserTR;							//��Դ��ʶ
	UINT							uUserBL;							//��Դ��ʶ
	UINT							uUserBM;							//��Դ��ʶ
	UINT							uUserBR;							//��Դ��ʶ

	//������Դ
	UINT							uInputML;							//��Դ��ʶ
	UINT							uInputMR;							//��Դ��ʶ
	UINT							uInputTL;							//��Դ��ʶ
	UINT							uInputTM;							//��Դ��ʶ
	UINT							uInputTR;							//��Դ��ʶ
	UINT							uInputBL;							//��Դ��ʶ
	UINT							uInputBM;							//��Դ��ʶ
	UINT							uInputBR;							//��Դ��ʶ

	//��ť��Դ
	UINT							uBtAddUser;							//��Դ��ʶ
	UINT							uBtColorSet;						//��Դ��ʶ
	UINT							uBtLeaveChat;						//��Դ��ʶ
	UINT							uBtExpression;						//��Դ��ʶ
	UINT							uBtCleanScreen;						//��Դ��ʶ
	UINT							uBtMoreFunction;					//��Դ��ʶ

	//���ư�ť
	UINT							uBtSendControl;						//��Դ��ʶ
};

//�û���Ϣ
struct tagPlazaUserInfoImage
{
	//��ɫ��Ϣ
	COLORREF						crHyperBack;						//��ɫ����
	COLORREF						crHyperText;						//��ɫ����
	COLORREF						crUserInfoText;						//��ɫ����

	//�û���Ϣ
	UINT							uUserInfoL;							//��Դ��ʶ
	UINT							uUserInfoM;							//��Դ��ʶ
	UINT							uUserInfoR;							//��Դ��ʶ
	UINT							uBtUserInfoMenu;					//��Դ��ʶ
};

//��������
struct tagPlazaCollocateImage
{
	//���ñ���
	UINT							uTitleML;							//��Դ��ʶ
	UINT							uTitleMR;							//��Դ��ʶ
	UINT							uTitleTL;							//��Դ��ʶ
	UINT							uTitleTM;							//��Դ��ʶ
	UINT							uTitleTR;							//��Դ��ʶ
	UINT							uTitleBL;							//��Դ��ʶ
	UINT							uTitleBM;							//��Դ��ʶ
	UINT							uTitleBR;							//��Դ��ʶ

	//������Դ
	UINT							uCollocateBar;						//��Դ��ʶ
	LPCTSTR							pszBtCollocateBar;					//��Դ��ʶ

	//��ɫ����
	COLORREF						crItemTitleTX;						//��ɫ����
	COLORREF						crItemHoverTX;						//��ɫ����
	COLORREF						crItemActiveTX;						//��ɫ����
	COLORREF						crItemNormalTX;						//��ɫ����
};

//����ר��
struct tagPlazaServiceImage
{
	//�������
	UINT							uTitleML;							//��Դ��ʶ
	UINT							uTitleMR;							//��Դ��ʶ
	UINT							uTitleTL;							//��Դ��ʶ
	UINT							uTitleTM;							//��Դ��ʶ
	UINT							uTitleTR;							//��Դ��ʶ
	UINT							uTitleBL;							//��Դ��ʶ
	UINT							uTitleBM;							//��Դ��ʶ
	UINT							uTitleBR;							//��Դ��ʶ

	//������Դ
	UINT							uUserServiceBar;					//��Դ��ʶ
	UINT							uMemberServiceBar;					//��Դ��ʶ

	//��ɫ����
	COLORREF						crServiceItemTX;					//��ɫ����
	COLORREF						crServiceItemBK;					//��ɫ����
};

//��ȫ����
struct tagPlazaSafePromptImage
{
	UINT							uBtProtect;							//��Դ��ʶ
	UINT							uBtBind;							//��Դ��ʶ
};

//������Դ
struct tagBugleImage
{
	UINT							uBtColorSet;						//��Դ��ʶ
	UINT							uBtExpression;						//��Դ��ʶ
};

//������Դ
struct tagDownLoadImage
{
	UINT							uBtLoadLast;						//��Դ��ʶ
	UINT							uBtLoadNext;						//��Դ��ʶ
};
//////////////////////////////////////////////////////////////////////////

//�༭��Դ
struct tagSkinEditResource
{
	COLORREF						crFocusTX;							//������ɫ
	COLORREF						crFocusBK;							//������ɫ
	COLORREF						crNoFocusTX;						//������ɫ
	COLORREF						crNoFocusBK;						//������ɫ
	COLORREF						crDisFocusTX;						//������ɫ
	COLORREF						crDisFocusBK;						//������ɫ
	COLORREF						crBoradNormal;						//�߿���ɫ
};

//�˵���Դ
struct tagSkinMenuResource
{
	//��Դ��Ϣ
	UINT							uFlags;								//������Ϣ

	//������ɫ
	COLORREF						crNormalText;						//�˵���ɫ
	COLORREF						crSelectText;						//�˵���ɫ

	//�߿���ɫ
	COLORREF						crMenuBar;							//�˵���ɫ
	COLORREF						crSeparator;						//�˵���ɫ
	COLORREF						crNormalBack;						//�˵���ɫ
	COLORREF						crSelectBack;						//�˵���ɫ
	COLORREF						crSelectBorder;						//�˵���ɫ

	//λ����Ϣ
	UINT							uItemHeight;						//�˵��߶�
	UINT							uMenuBarWidth;						//ͼ����
};

//��ť��Դ
struct tagSkinButtonResource
{
	UINT							uButton;							//������Ϣ
	COLORREF						crButtonText;						//������ɫ
};

//ѡ����Դ
struct tagSkinComboBoxResource
{
	UINT							uThumb;								//������Ϣ
	COLORREF						crBoradHover;						//�߿���ɫ
	COLORREF						crBoradNormal;						//�߿���ɫ
};

//������Դ
struct tagSkinDialogResource
{
	//������Ϣ
	bool							bShowIcon;							//��ʾͼ��
	bool							bShowTitle;							//��ʾ����
	HICON							hDefaultIcon;						//Ĭ��ͼ��

	//ƫ����Ϣ
	INT								nXIconExcursion;					//ͼ��ƫ��
	INT								nYIconExcursion;					//ͼ��ƫ��
	INT								nXButtonExcursion;					//��ťƫ��
	INT								nYButtonExcursion;					//��ťƫ��
	INT								nXTitleTextExcursion;				//����ƫ��
	INT								nYTitleTextExcursion;				//����ƫ��

	//��ɫ��Ϣ
	COLORREF						crTitleText;						//������ɫ
	COLORREF						crBackGround;						//������ɫ
	COLORREF						crControlText;						//������ɫ

	//������Ϣ
	UINT							uImageTL;							//������Ϣ
	UINT							uImageTM;							//������Ϣ
	UINT							uImageTR;							//������Ϣ
	UINT							uImageMR;							//������Ϣ
	UINT							uImageML;							//������Ϣ
	UINT							uImageBL;							//������Ϣ
	UINT							uImageBM;							//������Ϣ
	UINT							uImageBR;							//������Ϣ

	//������Ϣ
	UINT							uButtonMin;							//������Ϣ
	UINT							uButtonMax;							//������Ϣ
	UINT							uButtonRes;							//������Ϣ
	UINT							uButtonClose;						//������Ϣ
};

//�б���Դ
struct tagSkinListCtrlResource
{
	//��ɫ��Ϣ
	COLORREF						crTitle;							//������ɫ
	COLORREF						crListTX;							//�б�����
	COLORREF						crListBK;							//�б��ɫ

	//��Դ��Ϣ
	UINT							uImageL;							//������Ϣ
	UINT							uImageM;							//������Ϣ
	UINT							uImageR;							//������Ϣ
};

//������Դ
struct tagSkinScrollResource 
{
	UINT							uScrollImage;						//������Ϣ
};

//�����Դ
struct tagSkinSplitterResource
{
	COLORREF						crBorad;							//�߿���ɫ
	COLORREF						crBackGround;						//������ɫ
};

//�ȵ���Դ
struct tagSkinHyperLinkResource
{
	COLORREF						crBackColor;						//������ɫ
	COLORREF						crHoverText;						//������ɫ
	COLORREF						crNormalText;						//������ɫ
	COLORREF						crVisitedText;						//������ɫ
};

//ҳ����Դ
struct tagSkinTabControlResource
{
	COLORREF						crNormalTX;							//������ɫ
	COLORREF						crSelectTX;							//������ɫ
	COLORREF						crControlBack;						//������ɫ
	COLORREF						crControlBorad;						//�߿���ɫ
};

//������Դ
struct tagSkinEncircleResource
{
	UINT							uImageML;							//��Դ��ʶ
	UINT							uImageMR;							//��Դ��ʶ
	UINT							uImageTL;							//��Դ��ʶ
	UINT							uImageTM;							//��Դ��ʶ
	UINT							uImageTR;							//��Դ��ʶ
	UINT							uImageBL;							//��Դ��ʶ
	UINT							uImageBM;							//��Դ��ʶ
	UINT							uImageBR;							//��Դ��ʶ
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IPlazaResource INTERFACE_VERSION(1,1)
	static const GUID IID_IPlazaResource={0xb532bc36,0xb955,0x4bdb,0x9f,0xb0,0x78,0x81,0x36,0x1b,0x2d,0x2d};
#else
	#define VER_IPlazaResource INTERFACE_VERSION(1,1)
	static const GUID IID_IPlazaResource={0x27eaccdd,0x9076,0x4c10,0x89,0xde,0x11,0xf7,0x5e,0x88,0x9f,0x3a};
#endif

//������Դ
interface IPlazaResource : public IUnknownEx
{
	//�ں˽ӿ�
public:
	//��Դ���
	virtual HINSTANCE __cdecl GetResInstance()=NULL;

	//��Դ��Ϣ
public:
	//��������
	virtual VOID __cdecl GetPlazaChatImage(tagPlazaChatImage & ResourceInfo)=NULL;
	//��������
	virtual VOID __cdecl GetPlazaTableImage(tagPlazaTableImage & ResourceInfo)=NULL;
	//�������
	virtual VOID __cdecl GetPlazaFrameImage(tagPlazaFrameImage & ResourceInfo)=NULL;
	//�����б�
	virtual VOID __cdecl GetPlazaServerImage(tagPlazaServerImage & ResourceInfo)=NULL;
	//�������
	virtual VOID __cdecl GetPlazaBrowserImage(tagPlazaBrowserImage & ResourceInfo)=NULL;
	//��������
	virtual VOID __cdecl GetPlazaServiceImage(tagPlazaServiceImage & ResourceInfo)=NULL;
	//����˽��
	virtual VOID __cdecl GetPlazaWhisperImage(tagPlazaWhisperImage & ResourceInfo)=NULL;
	//�����û�
	virtual VOID __cdecl GetPlazaUserListImage(tagPlazaUserListImage & ResourceInfo)=NULL;
	//�����û�
	virtual VOID __cdecl GetPlazaUserInfoImage(tagPlazaUserInfoImage & ResourceInfo)=NULL;
	//��������
	virtual VOID __cdecl GetPlazaCollocateImage(tagPlazaCollocateImage & ResourceInfo)=NULL;
	//��ȫ����
	virtual VOID __cdecl GetPlazaSafePromptImage(tagPlazaSafePromptImage & ResourceInfo)=NULL;
	//������Դ
	virtual VOID __cdecl GetBugleImage(tagBugleImage &ResourceInfo)=NULL;
	//������Դ
	virtual VOID __cdecl GetDownloadImage(tagDownLoadImage &ResourceInfo)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IFrameResource INTERFACE_VERSION(1,1)
	static const GUID IID_IFrameResource={0xbb80ea74,0x4259,0x4180,0x9f,0xb4,0xd9,0x37,0x3a,0x5f,0xab,0xb9};
#else
	#define VER_IFrameResource INTERFACE_VERSION(1,1)
	static const GUID IID_IFrameResource={0x8512b9a8,0x40f6,0x426a,0xa7,0xe7,0x25,0x59,0xfe,0x88,0xb5,0x3f};
#endif

//�����Դ
interface IFrameResource : public IUnknownEx
{
	//�ں˽ӿ�
public:
	//��Դ���
	virtual HINSTANCE __cdecl GetResInstance()=NULL;

	//��Դ��Ϣ
public:
	//��Ϸ����
	virtual VOID __cdecl GetGameBarImage(tagGameBarImage & ResourceInfo)=NULL;
	//��Ϸ���
	virtual VOID __cdecl GetGameBillImage(tagGameBillImage & ResourceInfo)=NULL;
	//��Ϸ����
	virtual VOID __cdecl GetGameChatImage(tagGameChatImage & ResourceInfo)=NULL;
	//��Ϸ���
	virtual VOID __cdecl GetGameFrameImage(tagGameFrameImage & ResourceInfo)=NULL;
	//��Ϸ�û�
	virtual VOID __cdecl GetGameUserInfoImage(tagGameUserInfoImage & ResourceInfo)=NULL;
	//��Ϸ�б�
	virtual VOID __cdecl GetGameUserListImage(tagGameUserListImage & ResourceInfo)=NULL;
	//������Դ
	virtual VOID __cdecl GetBugleImage(tagBugleImage &ResourceInfo)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ISkinResource INTERFACE_VERSION(1,1)
	static const GUID IID_ISkinResource={0x24775ee6,0x729c,0x4b60,0x0098,0x54,0xd8,0xb7,0x4c,0x27,0x25,0x8e};
#else
	#define VER_ISkinResource INTERFACE_VERSION(1,1)
	static const GUID IID_ISkinResource={0xcaf877cf,0x37ca,0x44f0,0x00af,0x14,0x85,0xf5,0x67,0xd2,0x55,0x14};
#endif

//������Դ
interface ISkinResource : public IUnknownEx
{
	//�ں˽ӿ�
public:
	//��Դ���
	virtual HINSTANCE __cdecl GetResInstance()=NULL;

	//������Դ
public:
	//��������
	virtual bool __cdecl GetFontResource(CFont & Font)=NULL;
	//��ȡ��Ϣ
	virtual bool __cdecl GetEncircleResource(tagSkinEncircleResource & SkinEncircleResource)=NULL;

	//������Դ
public:
	//��ȡ��Ϣ
	virtual bool __cdecl GetMenuResource(tagSkinMenuResource & ResourceInfo)=NULL;
	//��ȡ��Ϣ
	virtual bool __cdecl GetEditResource(tagSkinEditResource & ResourceInfo)=NULL;
	//��ȡ��Ϣ
	virtual bool __cdecl GetDialogResource(tagSkinDialogResource & ResourceInfo)=NULL;
	//��ȡ��Ϣ
	virtual bool __cdecl GetButtonResource(tagSkinButtonResource & ResourceInfo)=NULL;
	//��ȡ��Ϣ
	virtual bool __cdecl GetComboBoxResource(tagSkinComboBoxResource & ResourceInfo)=NULL;
	//��ȡ��Ϣ
	virtual bool __cdecl GetSplitterResource(tagSkinSplitterResource & ResourceInfo)=NULL;
	//��ȡ��Ϣ
	virtual bool __cdecl GetListCtrlResource(tagSkinListCtrlResource & ResourceInfo)=NULL;
	//��ȡ��Ϣ
	virtual bool __cdecl GetHyperLinkResource(tagSkinHyperLinkResource & ResourceInfo)=NULL;
	//��ȡ��Ϣ
	virtual bool __cdecl GetTabControlResource(tagSkinTabControlResource & ResourceInfo)=NULL;
	//��ȡ��Ϣ
	virtual bool __cdecl GetScrollResource(tagSkinScrollResource & ResourceInfo)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IPlatformResource INTERFACE_VERSION(1,1)
	static const GUID IID_IPlatformResource={0x82ad737b,0xc01e,0x481f,0x0082,0x60,0x8e,0x45,0x07,0xef,0xaf,0x8e};
#else
	#define VER_IPlatformResource INTERFACE_VERSION(1,1)
	static const GUID IID_IPlatformResource={0x7fbe972b,0x898f,0x4429,0x00ab,0xfb,0x91,0xec,0x89,0x99,0x25,0x24};
#endif

//��Դ����
interface IPlatformResource : public IUnknownEx
{
	//�ں˽ӿ�
public:
	//��Դ���
	virtual HINSTANCE __cdecl GetResInstance()=NULL;
	//��ȡԤ��
	virtual VOID __cdecl GetSkinPreview(CImage & ImagePreview)=NULL;

	//������Դ
public:
	//������Դ
	virtual VOID * __cdecl GetSkinResource(REFGUID Guid, DWORD dwQueryVer)=NULL;
	//������Դ
	virtual VOID * __cdecl GetPlazaResource(REFGUID Guid, DWORD dwQueryVer)=NULL;
	//�����Դ
	virtual VOID * __cdecl GetFrameResource(REFGUID Guid, DWORD dwQueryVer)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//���������
DECLARE_MODULE_DYNAMIC(SkinResource, SkinResource)
DECLARE_MODULE_DYNAMIC(PlazaResource, PlazaResource)
DECLARE_MODULE_DYNAMIC(FrameResource, FrameResource)
DECLARE_MODULE_DYNAMIC(PlatformResource, PlatformResource)

//����ģ��
#ifndef _DEBUG
	#define RESOURCE_MODELE			TEXT("BuleResource.dll")			//ģ������
#else
	#define RESOURCE_MODELE			TEXT("BuleResourceD.dll")			//ģ������
#endif

//��ԴĿ¼
#define SKIN_RESOURCE				TEXT("SkinResource\\��ɫ����")		//��ԴĿ¼		

//��������
#define MODULE_CREATE_FUNCTION		"CreatePlatformResource"			//��������

//////////////////////////////////////////////////////////////////////////

#endif