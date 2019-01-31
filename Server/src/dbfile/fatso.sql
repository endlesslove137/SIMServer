/*
Navicat MySQL Data Transfer

Source Server         : fatso外网
Source Server Version : 50067
Source Host           : 115.238.54.88:3306
Source Database       : fatso

Target Server Type    : MYSQL
Target Server Version : 50067
File Encoding         : 65001

Date: 2015-08-20 15:01:06
*/
drop DATABASE if EXISTS `fatso`;
create database fatso character set utf8;
use fatso;

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for t_charge_order
-- ----------------------------
DROP TABLE IF EXISTS `t_charge_order`;
CREATE TABLE `t_charge_order` (
  `id` int(11) NOT NULL auto_increment,
  `pay_code` char(20) default '0',
  `pay_time` datetime default '0000-00-00 00:00:00',
  `device_id` char(50) default NULL,
  `order_id` char(30) default NULL,
  `sdk` char(20) default NULL,
  `pay_type` int(11) default '0',
  `amount` int(11) default '0',
  `order_result` int(11) default '0',
  `insert_time` datetime default '0000-00-00 00:00:00',
  PRIMARY KEY  (`id`),
  KEY `pay_time` (`pay_time`),
  KEY `pay_code` (`pay_code`),
  KEY `pay_order` (`order_id`)
) ENGINE=InnoDB AUTO_INCREMENT=149 DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records of t_charge_order
-- ----------------------------

-- ----------------------------
-- Table structure for t_user
-- ----------------------------
DROP TABLE IF EXISTS `t_user`;
CREATE TABLE `t_user` (
  `id` int(11) NOT NULL auto_increment,
  `deviceid` varchar(45) default '0',
  `createtime` timestamp NULL default '0000-00-00 00:00:00',
  `lastlogintime` timestamp NULL default '0000-00-00 00:00:00',
  `hiscore` int(11) default '0',
  `skillstate` int(11) default '0',
  `scoreTime` timestamp NULL default '0000-00-00 00:00:00',
  `mac` varchar(45) default NULL,
  `ip` varchar(45) default NULL,
  `CharName` varchar(40) default NULL,
  PRIMARY KEY  (`id`),
  UNIQUE KEY `deviceid_UNIQUE` (`deviceid`),
  KEY `score` (`hiscore`)
) ENGINE=InnoDB AUTO_INCREMENT=838 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of t_user
-- ----------------------------
INSERT INTO `t_user` VALUES ('86', '869630016246013', '2014-12-01 17:18:54', '2015-01-08 11:41:41', '10', '0', '2014-11-24 11:04:29', 'c46ab7bc5507', '014023095098', '钱包');
INSERT INTO `t_user` VALUES ('92', '', '2014-12-03 14:43:28', '2015-07-12 07:57:49', '6', '0', '2014-12-03 15:08:06', '', '112096165107', '支付宝');
INSERT INTO `t_user` VALUES ('130', '864502020745436', '2014-12-11 16:06:23', '2015-02-12 19:15:39', '14', '0', '2015-01-13 22:51:37', 'cca223d57a6f', '220175129231', '射了');
INSERT INTO `t_user` VALUES ('473', '864279021891176', '2015-02-22 15:56:31', '2015-04-03 12:15:14', '34', '0', '2015-03-03 21:39:57', '74adb77537f6', '117146146250', '战将');
INSERT INTO `t_user` VALUES ('494', '862741020069379', '2015-02-28 13:55:14', '2015-03-09 21:59:51', '22', '0', '2015-03-07 14:10:44', 'b8b42e8a39aa', '106046089092', '玻璃瓶');
INSERT INTO `t_user` VALUES ('558', '862751025755252', '2015-03-29 01:25:49', '2015-04-02 22:10:23', '32', '0', '2015-03-31 23:32:20', '000000000000', '117136011065', '瘦马');
INSERT INTO `t_user` VALUES ('607', 'A00000491AC8B7', '2015-05-02 19:35:26', '2015-05-04 15:51:41', '16', '0', '2015-05-02 19:42:41', '00664bf798e8', '123151136156', '快枪手');
INSERT INTO `t_user` VALUES ('756', '866697023928357', '2015-07-01 10:50:10', '2015-07-06 20:01:42', '30', '0', '2015-07-06 12:35:54', 'c4072fe08acf', '061131058104', '大神');
INSERT INTO `t_user` VALUES ('824', '866697024142099', '2015-07-16 10:23:23', '2015-07-16 10:23:23', '26', '0', '2015-07-06 12:35:54', 'c4072fe08acf', '175043189137', '超级无敌');
INSERT INTO `t_user` VALUES ('825', '00:30:18:A9:DC:F0', '2015-08-06 16:56:21', '2015-08-14 10:45:32', '12', '0', '2015-07-30 16:42:25', null, null, '王润智');

-- ----------------------------
-- Table structure for t_user_bak
-- ----------------------------
DROP TABLE IF EXISTS `t_user_bak`;
CREATE TABLE `t_user_bak` (
  `id` int(11) NOT NULL default '0',
  `deviceid` varchar(45) character set latin1 default '0',
  `createtime` timestamp NULL default '0000-00-00 00:00:00',
  `lastlogintime` timestamp NULL default '0000-00-00 00:00:00',
  `hiscore` int(11) default '0',
  `skillstate` int(11) default '0',
  `scoreTime` timestamp NULL default '0000-00-00 00:00:00',
  `mac` varchar(45) character set latin1 default NULL,
  `ip` varchar(45) character set latin1 default NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_general_ci;

-- ----------------------------
-- Records of t_user_bak
-- ----------------------------

-- ----------------------------
-- Table structure for vesion
-- ----------------------------
DROP TABLE IF EXISTS `vesion`;
CREATE TABLE `vesion` (
  `id` int(11) NOT NULL auto_increment,
  `ver_num` int(11) default '0',
  `ver_url` char(200) collate latin1_general_ci default NULL,
  `release_date` datetime default '0000-00-00 00:00:00',
  `expired_date` datetime default '0000-00-00 00:00:00',
  PRIMARY KEY  (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_general_ci;

-- ----------------------------
-- Records of vesion
-- ----------------------------
