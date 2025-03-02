USE LHPlatformDB

DELETE FROM [dbo].[GameConfigItem]

INSERT GameConfigItem (ConfigName,ConfigMask,ServiceMask)  VALUES ( '大厅版本',256,4)
INSERT GameConfigItem (ConfigName,ConfigMask,ServiceMask)  VALUES ( '低保参数',16,2)
INSERT GameConfigItem (ConfigName,ConfigMask,ServiceMask)  VALUES ( '黑白名单',32,2)
INSERT GameConfigItem (ConfigName,ConfigMask,ServiceMask)  VALUES ( '机器参数',8,2)
INSERT GameConfigItem (ConfigName,ConfigMask,ServiceMask)  VALUES ( '敏感词汇',512,6)
INSERT GameConfigItem (ConfigName,ConfigMask,ServiceMask)  VALUES ( '跑马灯配置',131072,6)
INSERT GameConfigItem (ConfigName,ConfigMask,ServiceMask)  VALUES ( '游戏版本',128,2)
INSERT GameConfigItem (ConfigName,ConfigMask,ServiceMask)  VALUES ( '游戏等级',2,6)
INSERT GameConfigItem (ConfigName,ConfigMask,ServiceMask)  VALUES ( '游戏列表',1,4)
INSERT GameConfigItem (ConfigName,ConfigMask,ServiceMask)  VALUES ( '游戏物品',4,6)
INSERT GameConfigItem (ConfigName,ConfigMask,ServiceMask)  VALUES ( '约战配置',64,2)
INSERT GameConfigItem (ConfigName,ConfigMask,ServiceMask)  VALUES ( '转盘配置',65536,2)
INSERT GameConfigItem (ConfigName,ConfigMask,ServiceMask)  VALUES ( '转账配置',1024,4)