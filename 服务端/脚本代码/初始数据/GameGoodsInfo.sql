USE LHPlatformDB

DELETE FROM dbo.GameGoodsInfo

INSERT GameGoodsInfo (GoodsID,OrderID,ImageID,HasIndate,GoodsKind,GoodsName,CanTradeType,CanStorePlace,GoodsOption,GoodsDescribe)  VALUES ( 1,0,0,0,1,'钻石',1,0,'','')
INSERT GameGoodsInfo (GoodsID,OrderID,ImageID,HasIndate,GoodsKind,GoodsName,CanTradeType,CanStorePlace,GoodsOption,GoodsDescribe)  VALUES ( 2,0,0,0,2,'黄金',1,31,'','')
INSERT GameGoodsInfo (GoodsID,OrderID,ImageID,HasIndate,GoodsKind,GoodsName,CanTradeType,CanStorePlace,GoodsOption,GoodsDescribe)  VALUES ( 3,0,0,0,3,'比赛券',1,0,'','')
INSERT GameGoodsInfo (GoodsID,OrderID,ImageID,HasIndate,GoodsKind,GoodsName,CanTradeType,CanStorePlace,GoodsOption,GoodsDescribe)  VALUES ( 4,0,0,0,4,'爱心',1,0,'','')