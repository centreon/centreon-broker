/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `cfg_acl_actions`
--

DROP TABLE IF EXISTS `cfg_acl_actions`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_acl_actions` (
  `acl_action_id` int(11) NOT NULL AUTO_INCREMENT,
  `acl_action_name` varchar(255) DEFAULT NULL,
  `acl_action_description` varchar(255) DEFAULT NULL,
  `acl_action_activate` enum('0','1','2') DEFAULT '1',
  `organization_id` int(11) NOT NULL,
  PRIMARY KEY (`acl_action_id`),
  KEY `FI__actions_ibfk_1` (`organization_id`),
  CONSTRAINT `acl_actions_ibfk_1` FOREIGN KEY (`organization_id`) REFERENCES `cfg_organizations` (`organization_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_acl_actions_rules`
--

DROP TABLE IF EXISTS `cfg_acl_actions_rules`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_acl_actions_rules` (
  `aar_id` int(11) NOT NULL AUTO_INCREMENT,
  `acl_action_rule_id` int(11) DEFAULT NULL,
  `acl_action_name` varchar(255) DEFAULT NULL,
  `organization_id` int(11) NOT NULL,
  PRIMARY KEY (`aar_id`),
  KEY `acl_action_rule_id` (`acl_action_rule_id`),
  KEY `FI__actions_rules_ibfk_2` (`organization_id`),
  CONSTRAINT `acl_actions_rules_ibfk_1` FOREIGN KEY (`acl_action_rule_id`) REFERENCES `cfg_acl_actions` (`acl_action_id`) ON DELETE CASCADE,
  CONSTRAINT `acl_actions_rules_ibfk_2` FOREIGN KEY (`organization_id`) REFERENCES `cfg_organizations` (`organization_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_acl_group_actions_relations`
--

DROP TABLE IF EXISTS `cfg_acl_group_actions_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_acl_group_actions_relations` (
  `agar_id` int(11) NOT NULL AUTO_INCREMENT,
  `acl_action_id` int(11) DEFAULT NULL,
  `acl_group_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`agar_id`),
  KEY `acl_action_id` (`acl_action_id`),
  KEY `acl_group_id` (`acl_group_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_acl_group_contacts_relations`
--

DROP TABLE IF EXISTS `cfg_acl_group_contacts_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_acl_group_contacts_relations` (
  `agcr_id` int(11) NOT NULL AUTO_INCREMENT,
  `contact_contact_id` int(11) DEFAULT NULL,
  `acl_group_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`agcr_id`),
  KEY `contact_contact_id` (`contact_contact_id`),
  KEY `acl_group_id` (`acl_group_id`),
  CONSTRAINT `acl_group_contacts_relations_ibfk_1` FOREIGN KEY (`contact_contact_id`) REFERENCES `cfg_contacts` (`contact_id`) ON DELETE CASCADE,
  CONSTRAINT `acl_group_contacts_relations_ibfk_2` FOREIGN KEY (`acl_group_id`) REFERENCES `cfg_acl_groups` (`acl_group_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_acl_groups`
--

DROP TABLE IF EXISTS `cfg_acl_groups`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_acl_groups` (
  `acl_group_id` int(11) NOT NULL AUTO_INCREMENT,
  `acl_group_name` varchar(255) DEFAULT NULL,
  `acl_group_alias` varchar(255) DEFAULT NULL,
  `acl_group_changed` int(11) NOT NULL,
  `acl_group_activate` enum('0','1','2') DEFAULT '1',
  PRIMARY KEY (`acl_group_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_acl_groups_menus_relations`
--

DROP TABLE IF EXISTS `cfg_acl_groups_menus_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_acl_groups_menus_relations` (
  `acl_group_id` int(11) NOT NULL,
  `acl_menu_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`acl_group_id`,`acl_menu_id`),
  KEY `acl_groups_menus_relations_id_1` (`acl_menu_id`),
  CONSTRAINT `acl_groups_menus_relations_ibfk_1` FOREIGN KEY (`acl_group_id`) REFERENCES `cfg_acl_groups` (`acl_group_id`) ON DELETE CASCADE,
  CONSTRAINT `acl_groups_menus_relations_ibfk_2` FOREIGN KEY (`acl_menu_id`) REFERENCES `cfg_acl_menus` (`acl_menu_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_acl_menu_menu_relations`
--

DROP TABLE IF EXISTS `cfg_acl_menu_menu_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_acl_menu_menu_relations` (
  `acl_menu_id` int(10) unsigned NOT NULL DEFAULT '0',
  `menu_id` int(10) unsigned NOT NULL DEFAULT '0',
  `acl_level` tinyint(3) DEFAULT NULL,
  PRIMARY KEY (`acl_menu_id`,`menu_id`),
  KEY `acl_menu_menu_relations_ibfk_2` (`menu_id`),
  CONSTRAINT `acl_menu_menu_relations_ibfk_1` FOREIGN KEY (`acl_menu_id`) REFERENCES `cfg_acl_menus` (`acl_menu_id`) ON DELETE CASCADE,
  CONSTRAINT `acl_menu_menu_relations_ibfk_2` FOREIGN KEY (`menu_id`) REFERENCES `cfg_menus` (`menu_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_acl_menus`
--

DROP TABLE IF EXISTS `cfg_acl_menus`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_acl_menus` (
  `acl_menu_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL,
  `description` varchar(255) NOT NULL,
  `enabled` tinyint(1) DEFAULT '1',
  PRIMARY KEY (`acl_menu_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_acl_res_group_relations`
--

DROP TABLE IF EXISTS `cfg_acl_res_group_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_acl_res_group_relations` (
  `argr_id` int(11) NOT NULL AUTO_INCREMENT,
  `acl_res_id` int(11) DEFAULT NULL,
  `acl_group_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`argr_id`),
  KEY `acl_res_id` (`acl_res_id`),
  KEY `acl_group_id` (`acl_group_id`),
  CONSTRAINT `acl_res_group_relations_ibfk_1` FOREIGN KEY (`acl_res_id`) REFERENCES `cfg_acl_resources` (`acl_res_id`) ON DELETE CASCADE,
  CONSTRAINT `acl_res_group_relations_ibfk_2` FOREIGN KEY (`acl_group_id`) REFERENCES `cfg_acl_groups` (`acl_group_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_acl_resources`
--

DROP TABLE IF EXISTS `cfg_acl_resources`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_acl_resources` (
  `acl_res_id` int(11) NOT NULL AUTO_INCREMENT,
  `acl_res_name` varchar(255) DEFAULT NULL,
  `acl_res_alias` varchar(255) DEFAULT NULL,
  `all_hosts` enum('0','1') DEFAULT NULL,
  `all_hostgroups` enum('0','1') DEFAULT NULL,
  `all_servicegroups` enum('0','1') DEFAULT NULL,
  `acl_res_activate` enum('0','1','2') DEFAULT NULL,
  `acl_res_comment` text,
  `acl_res_status` enum('0','1') DEFAULT '1',
  `changed` int(11) DEFAULT NULL,
  PRIMARY KEY (`acl_res_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_acl_resources_hc_relations`
--

DROP TABLE IF EXISTS `cfg_acl_resources_hc_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_acl_resources_hc_relations` (
  `arhcr_id` int(11) NOT NULL AUTO_INCREMENT,
  `hc_id` int(11) DEFAULT NULL,
  `acl_res_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`arhcr_id`),
  KEY `hc_id` (`hc_id`),
  KEY `acl_res_id` (`acl_res_id`),
  CONSTRAINT `acl_resources_hc_relations_ibfk_1` FOREIGN KEY (`hc_id`) REFERENCES `cfg_hostcategories` (`hc_id`) ON DELETE CASCADE,
  CONSTRAINT `cfg_acl_resources_hc_relations_ibfk_2` FOREIGN KEY (`acl_res_id`) REFERENCES `cfg_acl_resources` (`acl_res_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_acl_resources_hg_relations`
--

DROP TABLE IF EXISTS `cfg_acl_resources_hg_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_acl_resources_hg_relations` (
  `arhge_id` int(11) NOT NULL AUTO_INCREMENT,
  `hg_hg_id` int(11) DEFAULT NULL,
  `acl_res_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`arhge_id`),
  KEY `hg_hg_id` (`hg_hg_id`),
  KEY `acl_res_id` (`acl_res_id`),
  KEY `hg_hg_id_2` (`hg_hg_id`,`acl_res_id`),
  CONSTRAINT `cfg_acl_resources_hg_relations_ibfk_1` FOREIGN KEY (`hg_hg_id`) REFERENCES `cfg_hostgroups` (`hg_id`) ON DELETE CASCADE,
  CONSTRAINT `cfg_acl_resources_hg_relations_ibfk_2` FOREIGN KEY (`acl_res_id`) REFERENCES `cfg_acl_resources` (`acl_res_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_acl_resources_hostex_relations`
--

DROP TABLE IF EXISTS `cfg_acl_resources_hostex_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_acl_resources_hostex_relations` (
  `arhe_id` int(11) NOT NULL AUTO_INCREMENT,
  `host_host_id` int(11) DEFAULT NULL,
  `acl_res_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`arhe_id`),
  KEY `host_host_id` (`host_host_id`),
  KEY `acl_res_id` (`acl_res_id`),
  CONSTRAINT `acl_resources_hostex_relations_ibfk_1` FOREIGN KEY (`host_host_id`) REFERENCES `cfg_hosts` (`host_id`) ON DELETE CASCADE,
  CONSTRAINT `cfg_acl_resources_hostex_relations_ibfk_2` FOREIGN KEY (`acl_res_id`) REFERENCES `cfg_acl_resources` (`acl_res_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_acl_resources_hosts_relations`
--

DROP TABLE IF EXISTS `cfg_acl_resources_hosts_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_acl_resources_hosts_relations` (
  `arhr_id` int(11) NOT NULL AUTO_INCREMENT,
  `host_host_id` int(11) DEFAULT NULL,
  `acl_res_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`arhr_id`),
  KEY `host_host_id` (`host_host_id`),
  KEY `acl_res_id` (`acl_res_id`),
  CONSTRAINT `acl_resources_host_relations_ibfk_1` FOREIGN KEY (`host_host_id`) REFERENCES `cfg_hosts` (`host_id`) ON DELETE CASCADE,
  CONSTRAINT `cfg_acl_resources_host_relations_ibfk_2` FOREIGN KEY (`acl_res_id`) REFERENCES `cfg_acl_resources` (`acl_res_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_acl_resources_meta_relations`
--

DROP TABLE IF EXISTS `cfg_acl_resources_meta_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_acl_resources_meta_relations` (
  `armse_id` int(11) NOT NULL AUTO_INCREMENT,
  `meta_id` int(11) DEFAULT NULL,
  `acl_res_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`armse_id`),
  KEY `meta_id` (`meta_id`),
  KEY `acl_res_id` (`acl_res_id`),
  CONSTRAINT `cfg_acl_resources_meta_relations_ibfk_1` FOREIGN KEY (`meta_id`) REFERENCES `cfg_meta_services` (`meta_id`) ON DELETE CASCADE,
  CONSTRAINT `cfg_acl_resources_meta_relations_ibfk_2` FOREIGN KEY (`acl_res_id`) REFERENCES `cfg_acl_resources` (`acl_res_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_acl_resources_pollers_relations`
--

DROP TABLE IF EXISTS `cfg_acl_resources_pollers_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_acl_resources_pollers_relations` (
  `arpr_id` int(11) NOT NULL AUTO_INCREMENT,
  `poller_id` int(11) DEFAULT NULL,
  `acl_res_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`arpr_id`),
  KEY `poller_id` (`poller_id`),
  KEY `acl_res_id` (`acl_res_id`),
  CONSTRAINT `cfg_acl_resources_pollers_relations_ibfk_1` FOREIGN KEY (`poller_id`) REFERENCES `cfg_pollers` (`poller_id`) ON DELETE CASCADE,
  CONSTRAINT `cfg_acl_resources_pollers_relations_ibfk_2` FOREIGN KEY (`acl_res_id`) REFERENCES `cfg_acl_resources` (`acl_res_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_acl_resources_sc_relations`
--

DROP TABLE IF EXISTS `cfg_acl_resources_sc_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_acl_resources_sc_relations` (
  `arscr_id` int(11) NOT NULL AUTO_INCREMENT,
  `sc_id` int(11) DEFAULT NULL,
  `acl_res_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`arscr_id`),
  KEY `sc_id` (`sc_id`),
  KEY `acl_res_id` (`acl_res_id`),
  CONSTRAINT `cfg_acl_resources_sc_relations_ibfk_1` FOREIGN KEY (`sc_id`) REFERENCES `cfg_servicecategories` (`sc_id`) ON DELETE CASCADE,
  CONSTRAINT `cfg_acl_resources_sc_relations_ibfk_2` FOREIGN KEY (`acl_res_id`) REFERENCES `cfg_acl_resources` (`acl_res_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_acl_resources_services_relations`
--

DROP TABLE IF EXISTS `cfg_acl_resources_services_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_acl_resources_services_relations` (
  `arsr_id` int(11) NOT NULL AUTO_INCREMENT,
  `service_service_id` int(11) DEFAULT NULL,
  `acl_group_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`arsr_id`),
  KEY `service_service_id` (`service_service_id`),
  KEY `acl_group_id` (`acl_group_id`),
  CONSTRAINT `acl_resources_service_relations_ibfk_1` FOREIGN KEY (`service_service_id`) REFERENCES `cfg_services` (`service_id`) ON DELETE CASCADE,
  CONSTRAINT `acl_resources_service_relations_ibfk_2` FOREIGN KEY (`acl_group_id`) REFERENCES `cfg_acl_groups` (`acl_group_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_acl_resources_sg_relations`
--

DROP TABLE IF EXISTS `cfg_acl_resources_sg_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_acl_resources_sg_relations` (
  `asgr` int(11) NOT NULL AUTO_INCREMENT,
  `sg_id` int(11) DEFAULT NULL,
  `acl_res_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`asgr`),
  KEY `sg_id` (`sg_id`),
  KEY `acl_res_id` (`acl_res_id`),
  KEY `sg_id_2` (`sg_id`,`acl_res_id`),
  CONSTRAINT `acl_resources_sg_relations_ibfk_1` FOREIGN KEY (`sg_id`) REFERENCES `cfg_servicegroups` (`sg_id`) ON DELETE CASCADE,
  CONSTRAINT `cfg_acl_resources_sg_relations_ibfk_2` FOREIGN KEY (`acl_res_id`) REFERENCES `cfg_acl_resources` (`acl_res_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_acl_routes`
--

DROP TABLE IF EXISTS `cfg_acl_routes`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_acl_routes` (
  `acl_route_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `route` varchar(255) NOT NULL,
  `permission` int(11) NOT NULL,
  `acl_group_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`acl_route_id`),
  KEY `acl_group_id` (`acl_group_id`),
  CONSTRAINT `fk_acl_route_group_id` FOREIGN KEY (`acl_group_id`) REFERENCES `cfg_acl_groups` (`acl_group_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_api_tokens`
--

DROP TABLE IF EXISTS `cfg_api_tokens`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_api_tokens` (
  `api_token_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `value` varchar(200) NOT NULL,
  `user_id` int(10) unsigned NOT NULL,
  `updatedat` datetime NOT NULL,
  PRIMARY KEY (`api_token_id`),
  UNIQUE KEY `value` (`value`),
  KEY `user_ibfk_1` (`user_id`),
  CONSTRAINT `user_ibfk_1` FOREIGN KEY (`user_id`) REFERENCES `cfg_users` (`user_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_auth_resources`
--

DROP TABLE IF EXISTS `cfg_auth_resources`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_auth_resources` (
  `ar_id` int(11) NOT NULL AUTO_INCREMENT,
  `ar_name` varchar(255) NOT NULL DEFAULT 'Default',
  `ar_description` varchar(255) NOT NULL DEFAULT 'Default description',
  `ar_type` varchar(50) NOT NULL,
  `ar_enable` enum('0','1') DEFAULT '0',
  PRIMARY KEY (`ar_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_auth_resources_info`
--

DROP TABLE IF EXISTS `cfg_auth_resources_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_auth_resources_info` (
  `ar_id` int(11) NOT NULL,
  `ari_name` varchar(100) NOT NULL,
  `ari_value` varchar(255) NOT NULL,
  PRIMARY KEY (`ar_id`,`ari_name`),
  CONSTRAINT `auth_resource_info_ibfk_1` FOREIGN KEY (`ar_id`) REFERENCES `cfg_auth_resources` (`ar_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_auth_resources_servers`
--

DROP TABLE IF EXISTS `cfg_auth_resources_servers`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_auth_resources_servers` (
  `ldap_server_id` int(11) NOT NULL AUTO_INCREMENT,
  `auth_resource_id` int(11) NOT NULL,
  `server_address` varchar(255) NOT NULL,
  `server_port` int(11) NOT NULL,
  `use_ssl` tinyint(4) DEFAULT '0',
  `use_tls` tinyint(4) DEFAULT '0',
  `server_order` tinyint(4) NOT NULL DEFAULT '1',
  PRIMARY KEY (`ldap_server_id`),
  KEY `fk_auth_resource_id` (`auth_resource_id`),
  CONSTRAINT `fk_auth_resource_id` FOREIGN KEY (`auth_resource_id`) REFERENCES `cfg_auth_resources` (`ar_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_binaries`
--

DROP TABLE IF EXISTS `cfg_binaries`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_binaries` (
  `binary_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `filename` varchar(255) NOT NULL,
  `checksum` varchar(255) NOT NULL,
  `mimetype` varchar(255) NOT NULL,
  `filetype` tinyint(1) NOT NULL DEFAULT '1',
  `binary_content` longblob NOT NULL,
  PRIMARY KEY (`binary_id`),
  UNIQUE KEY `binaries_idx01` (`checksum`,`mimetype`),
  UNIQUE KEY `binaries_idx02` (`filename`,`filetype`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_binary_type`
--

DROP TABLE IF EXISTS `cfg_binary_type`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_binary_type` (
  `binary_type_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `type_name` varchar(255) NOT NULL,
  `module_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`binary_type_id`),
  UNIQUE KEY `type_name` (`type_name`),
  KEY `module_id` (`module_id`),
  CONSTRAINT `binary_type_ibfk_1` FOREIGN KEY (`module_id`) REFERENCES `cfg_modules` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_binary_type_binaries_relations`
--

DROP TABLE IF EXISTS `cfg_binary_type_binaries_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_binary_type_binaries_relations` (
  `binary_type_id` int(10) unsigned NOT NULL,
  `binary_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`binary_type_id`,`binary_id`),
  KEY `binary_id` (`binary_id`),
  CONSTRAINT `binary_type_binaries_relation_ibfk_1` FOREIGN KEY (`binary_type_id`) REFERENCES `cfg_binary_type` (`binary_type_id`) ON DELETE CASCADE,
  CONSTRAINT `binary_type_binaries_relation_ibfk_2` FOREIGN KEY (`binary_id`) REFERENCES `cfg_binaries` (`binary_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_bookmarks`
--

DROP TABLE IF EXISTS `cfg_bookmarks`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_bookmarks` (
  `bookmark_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `user_id` int(10) unsigned NOT NULL,
  `label` varchar(255) NOT NULL,
  `type` varchar(255) NOT NULL,
  `quick_access` varchar(255) NOT NULL,
  `short_url_code` varchar(255) NOT NULL,
  `route` varchar(255) NOT NULL,
  `is_always_visible` tinyint(1) DEFAULT '0',
  `is_public` tinyint(1) DEFAULT '0',
  PRIMARY KEY (`bookmark_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_commands`
--

DROP TABLE IF EXISTS `cfg_commands`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_commands` (
  `command_id` int(11) NOT NULL AUTO_INCREMENT,
  `connector_id` int(10) unsigned DEFAULT NULL,
  `command_name` varchar(200) DEFAULT NULL,
  `command_line` text,
  `command_example` varchar(254) DEFAULT NULL,
  `command_type` tinyint(4) DEFAULT NULL,
  `enable_shell` int(1) unsigned NOT NULL DEFAULT '0',
  `command_comment` text,
  `graph_id` int(11) DEFAULT NULL,
  `cmd_cat_id` int(11) DEFAULT NULL,
  `organization_id` int(11) NOT NULL,
  PRIMARY KEY (`command_id`),
  UNIQUE KEY `command_name` (`command_name`),
  KEY `connector_id` (`connector_id`),
  KEY `FI_mand_ibfk_2` (`organization_id`),
  CONSTRAINT `command_ibfk_1` FOREIGN KEY (`connector_id`) REFERENCES `cfg_connectors` (`id`) ON DELETE SET NULL ON UPDATE CASCADE,
  CONSTRAINT `command_ibfk_2` FOREIGN KEY (`organization_id`) REFERENCES `cfg_organizations` (`organization_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_commands_args_description`
--

DROP TABLE IF EXISTS `cfg_commands_args_description`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_commands_args_description` (
  `cmd_id` int(11) NOT NULL,
  `macro_name` varchar(255) NOT NULL,
  `macro_description` varchar(255) NOT NULL,
  KEY `command_arg_description_ibfk_1` (`cmd_id`),
  CONSTRAINT `command_arg_description_ibfk_1` FOREIGN KEY (`cmd_id`) REFERENCES `cfg_commands` (`command_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_commands_categories`
--

DROP TABLE IF EXISTS `cfg_commands_categories`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_commands_categories` (
  `cmd_category_id` int(11) NOT NULL AUTO_INCREMENT,
  `category_name` varchar(255) NOT NULL,
  `category_alias` varchar(255) NOT NULL,
  `category_order` int(11) NOT NULL,
  PRIMARY KEY (`cmd_category_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_commands_categories_relations`
--

DROP TABLE IF EXISTS `cfg_commands_categories_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_commands_categories_relations` (
  `cmd_cat_id` int(11) NOT NULL AUTO_INCREMENT,
  `category_id` int(11) DEFAULT NULL,
  `command_command_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`cmd_cat_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_connectors`
--

DROP TABLE IF EXISTS `cfg_connectors`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_connectors` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL,
  `description` varchar(255) DEFAULT NULL,
  `command_line` varchar(512) NOT NULL,
  `enabled` int(1) unsigned NOT NULL DEFAULT '1',
  `created` int(10) unsigned NOT NULL,
  `modified` int(10) unsigned NOT NULL,
  `organization_id` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`),
  KEY `enabled` (`enabled`),
  KEY `FI_nectors_ibfk_1` (`organization_id`),
  CONSTRAINT `connectors_ibfk_1` FOREIGN KEY (`organization_id`) REFERENCES `cfg_organizations` (`organization_id`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_contacts`
--

DROP TABLE IF EXISTS `cfg_contacts`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_contacts` (
  `contact_id` int(11) NOT NULL AUTO_INCREMENT,
  `description` varchar(200) DEFAULT NULL,
  PRIMARY KEY (`contact_id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_contacts_infos`
--

DROP TABLE IF EXISTS `cfg_contacts_infos`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_contacts_infos` (
  `contact_info_id` int(11) NOT NULL AUTO_INCREMENT,
  `info_key` varchar(200) NOT NULL,
  `info_value` varchar(200) NOT NULL,
  `contact_id` int(11) NOT NULL,
  PRIMARY KEY (`contact_info_id`),
  KEY `contact_ibfk_1` (`contact_id`),
  CONSTRAINT `contact_ibfk_1` FOREIGN KEY (`contact_id`) REFERENCES `cfg_contacts` (`contact_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_cron_operation`
--

DROP TABLE IF EXISTS `cfg_cron_operation`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_cron_operation` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(254) DEFAULT NULL,
  `command` varchar(254) DEFAULT NULL,
  `time_launch` int(11) DEFAULT NULL,
  `last_modification` int(11) DEFAULT '0',
  `system` enum('0','1') DEFAULT NULL,
  `module` enum('0','1') DEFAULT NULL,
  `running` enum('0','1') DEFAULT NULL,
  `pid` int(11) DEFAULT NULL,
  `last_execution_time` int(11) NOT NULL,
  `activate` enum('0','1') DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_custom_views`
--

DROP TABLE IF EXISTS `cfg_custom_views`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_custom_views` (
  `custom_view_id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL,
  `mode` tinyint(2) DEFAULT '0',
  `locked` tinyint(2) DEFAULT '0',
  `owner_id` int(11) NOT NULL,
  `position` text,
  PRIMARY KEY (`custom_view_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_custom_views_default`
--

DROP TABLE IF EXISTS `cfg_custom_views_default`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_custom_views_default` (
  `user_id` int(11) NOT NULL,
  `custom_view_id` int(11) NOT NULL,
  KEY `fk_custom_view_default_user_id` (`user_id`),
  KEY `fk_custom_view_default_cv_id` (`custom_view_id`),
  CONSTRAINT `fk_custom_view_default_cv_id` FOREIGN KEY (`custom_view_id`) REFERENCES `cfg_custom_views` (`custom_view_id`) ON DELETE CASCADE,
  CONSTRAINT `fk_custom_view_default_user_id` FOREIGN KEY (`user_id`) REFERENCES `cfg_contacts` (`contact_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_custom_views_users_relations`
--

DROP TABLE IF EXISTS `cfg_custom_views_users_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_custom_views_users_relations` (
  `custom_view_id` int(11) NOT NULL,
  `user_id` int(10) unsigned NOT NULL,
  `is_default` tinyint(1) NOT NULL,
  PRIMARY KEY (`custom_view_id`,`user_id`),
  KEY `FI_custom_views_user_id` (`user_id`),
  CONSTRAINT `fk_custom_view_user_id` FOREIGN KEY (`custom_view_id`) REFERENCES `cfg_custom_views` (`custom_view_id`) ON DELETE CASCADE,
  CONSTRAINT `fk_custom_views_user_id` FOREIGN KEY (`user_id`) REFERENCES `cfg_users` (`user_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_customvariables_hosts`
--

DROP TABLE IF EXISTS `cfg_customvariables_hosts`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_customvariables_hosts` (
  `host_macro_id` int(11) NOT NULL AUTO_INCREMENT,
  `host_macro_name` varchar(255) NOT NULL,
  `host_macro_value` varchar(255) NOT NULL,
  `is_password` tinyint(2) DEFAULT NULL,
  `host_host_id` int(11) NOT NULL,
  PRIMARY KEY (`host_macro_id`),
  KEY `host_host_id` (`host_host_id`),
  CONSTRAINT `cfg_customvariables_hosts_ibfk_1` FOREIGN KEY (`host_host_id`) REFERENCES `cfg_hosts` (`host_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_customvariables_services`
--

DROP TABLE IF EXISTS `cfg_customvariables_services`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_customvariables_services` (
  `svc_macro_id` int(11) NOT NULL AUTO_INCREMENT,
  `svc_macro_name` varchar(255) NOT NULL,
  `svc_macro_value` varchar(255) NOT NULL,
  `is_password` tinyint(2) DEFAULT NULL,
  `svc_svc_id` int(11) NOT NULL,
  PRIMARY KEY (`svc_macro_id`),
  KEY `svc_svc_id` (`svc_svc_id`),
  CONSTRAINT `cfg_customvariables_services_ibfk_1` FOREIGN KEY (`svc_svc_id`) REFERENCES `cfg_services` (`service_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_dependencies`
--

DROP TABLE IF EXISTS `cfg_dependencies`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_dependencies` (
  `dep_id` int(11) NOT NULL AUTO_INCREMENT,
  `dep_name` varchar(255) DEFAULT NULL,
  `dep_description` varchar(255) DEFAULT NULL,
  `inherits_parent` enum('0','1') DEFAULT NULL,
  `execution_failure_criteria` varchar(255) DEFAULT NULL,
  `notification_failure_criteria` varchar(255) DEFAULT NULL,
  `dep_comment` text,
  `organization_id` int(11) NOT NULL,
  PRIMARY KEY (`dep_id`),
  KEY `FI_endencies_ibfk_1` (`organization_id`),
  CONSTRAINT `dependencies_ibfk_1` FOREIGN KEY (`organization_id`) REFERENCES `cfg_organizations` (`organization_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_dependencies_hostchildren_relations`
--

DROP TABLE IF EXISTS `cfg_dependencies_hostchildren_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_dependencies_hostchildren_relations` (
  `dhcr_id` int(11) NOT NULL AUTO_INCREMENT,
  `dependency_dep_id` int(11) DEFAULT NULL,
  `host_host_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`dhcr_id`),
  KEY `dependency_index` (`dependency_dep_id`),
  KEY `host_index` (`host_host_id`),
  CONSTRAINT `cfg_dependencies_hostchildren_relation_ibfk_1` FOREIGN KEY (`dependency_dep_id`) REFERENCES `cfg_dependencies` (`dep_id`) ON DELETE CASCADE,
  CONSTRAINT `dependency_hostchildren_relation_ibfk_2` FOREIGN KEY (`host_host_id`) REFERENCES `cfg_hosts` (`host_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_dependencies_hostgroupchildren_relations`
--

DROP TABLE IF EXISTS `cfg_dependencies_hostgroupchildren_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_dependencies_hostgroupchildren_relations` (
  `dhgcr_id` int(11) NOT NULL AUTO_INCREMENT,
  `dependency_dep_id` int(11) DEFAULT NULL,
  `hostgroup_hg_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`dhgcr_id`),
  KEY `dependency_index` (`dependency_dep_id`),
  KEY `hostgroup_index` (`hostgroup_hg_id`),
  CONSTRAINT `cfg_dependencies_hostgroupchildren_relation_ibfk_1` FOREIGN KEY (`dependency_dep_id`) REFERENCES `cfg_dependencies` (`dep_id`) ON DELETE CASCADE,
  CONSTRAINT `cfg_dependency_hostgroupchildren_relation_ibfk_2` FOREIGN KEY (`hostgroup_hg_id`) REFERENCES `cfg_hostgroups` (`hg_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_dependencies_hostgroupparents_relations`
--

DROP TABLE IF EXISTS `cfg_dependencies_hostgroupparents_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_dependencies_hostgroupparents_relations` (
  `dhgpr_id` int(11) NOT NULL AUTO_INCREMENT,
  `dependency_dep_id` int(11) DEFAULT NULL,
  `hostgroup_hg_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`dhgpr_id`),
  KEY `dependency_index` (`dependency_dep_id`),
  KEY `hostgroup_index` (`hostgroup_hg_id`),
  CONSTRAINT `cfg_dependencies_hostgroupParent_relation_ibfk_1` FOREIGN KEY (`dependency_dep_id`) REFERENCES `cfg_dependencies` (`dep_id`) ON DELETE CASCADE,
  CONSTRAINT `cfg_dependency_hostgroupParent_relation_ibfk_2` FOREIGN KEY (`hostgroup_hg_id`) REFERENCES `cfg_hostgroups` (`hg_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_dependencies_hostparents_relations`
--

DROP TABLE IF EXISTS `cfg_dependencies_hostparents_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_dependencies_hostparents_relations` (
  `dhpr_id` int(11) NOT NULL AUTO_INCREMENT,
  `dependency_dep_id` int(11) DEFAULT NULL,
  `host_host_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`dhpr_id`),
  KEY `dependency_index` (`dependency_dep_id`),
  KEY `host_index` (`host_host_id`),
  CONSTRAINT `cfg_dependencies_hostParent_relation_ibfk_1` FOREIGN KEY (`dependency_dep_id`) REFERENCES `cfg_dependencies` (`dep_id`) ON DELETE CASCADE,
  CONSTRAINT `dependency_hostParent_relation_ibfk_2` FOREIGN KEY (`host_host_id`) REFERENCES `cfg_hosts` (`host_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_dependencies_metaservicechildren_relations`
--

DROP TABLE IF EXISTS `cfg_dependencies_metaservicechildren_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_dependencies_metaservicechildren_relations` (
  `dmscr_id` int(11) NOT NULL AUTO_INCREMENT,
  `dependency_dep_id` int(11) DEFAULT NULL,
  `meta_service_meta_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`dmscr_id`),
  KEY `dependency_index` (`dependency_dep_id`),
  KEY `meta_service_index` (`meta_service_meta_id`),
  CONSTRAINT `cfg_dependencies_metaservicechildren_relation_ibfk_1` FOREIGN KEY (`dependency_dep_id`) REFERENCES `cfg_dependencies` (`dep_id`) ON DELETE CASCADE,
  CONSTRAINT `cfg_dependencies_metaservicechildren_relation_ibfk_2` FOREIGN KEY (`meta_service_meta_id`) REFERENCES `cfg_meta_services` (`meta_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_dependencies_metaserviceparents_relations`
--

DROP TABLE IF EXISTS `cfg_dependencies_metaserviceparents_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_dependencies_metaserviceparents_relations` (
  `dmspr_id` int(11) NOT NULL AUTO_INCREMENT,
  `dependency_dep_id` int(11) DEFAULT NULL,
  `meta_service_meta_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`dmspr_id`),
  KEY `dependency_index` (`dependency_dep_id`),
  KEY `meta_service_index` (`meta_service_meta_id`),
  CONSTRAINT `cfg_dependencies_metaserviceparents_relations_ibfk_1` FOREIGN KEY (`dependency_dep_id`) REFERENCES `cfg_dependencies` (`dep_id`) ON DELETE CASCADE,
  CONSTRAINT `cfg_dependencies_metaserviceparents_relations_ibfk_2` FOREIGN KEY (`meta_service_meta_id`) REFERENCES `cfg_meta_services` (`meta_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_dependencies_servicechildren_relations`
--

DROP TABLE IF EXISTS `cfg_dependencies_servicechildren_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_dependencies_servicechildren_relations` (
  `dscr_id` int(11) NOT NULL AUTO_INCREMENT,
  `dependency_dep_id` int(11) DEFAULT NULL,
  `service_service_id` int(11) DEFAULT NULL,
  `host_host_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`dscr_id`),
  KEY `dependency_index` (`dependency_dep_id`),
  KEY `service_index` (`service_service_id`),
  KEY `host_index` (`host_host_id`),
  CONSTRAINT `cfg_dependencies_servicechildren_relation_ibfk_1` FOREIGN KEY (`dependency_dep_id`) REFERENCES `cfg_dependencies` (`dep_id`) ON DELETE CASCADE,
  CONSTRAINT `dependency_servicechildren_relation_ibfk_2` FOREIGN KEY (`service_service_id`) REFERENCES `cfg_services` (`service_id`) ON DELETE CASCADE,
  CONSTRAINT `dependency_servicechildren_relation_ibfk_3` FOREIGN KEY (`host_host_id`) REFERENCES `cfg_hosts` (`host_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_dependencies_servicegroupchildren_relations`
--

DROP TABLE IF EXISTS `cfg_dependencies_servicegroupchildren_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_dependencies_servicegroupchildren_relations` (
  `dsgcr_id` int(11) NOT NULL AUTO_INCREMENT,
  `dependency_dep_id` int(11) DEFAULT NULL,
  `servicegroup_sg_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`dsgcr_id`),
  KEY `dependency_index` (`dependency_dep_id`),
  KEY `sg_index` (`servicegroup_sg_id`),
  CONSTRAINT `cfg_dependencies_servicegroupchildren_relation_ibfk_1` FOREIGN KEY (`dependency_dep_id`) REFERENCES `cfg_dependencies` (`dep_id`) ON DELETE CASCADE,
  CONSTRAINT `dependency_servicegroupchildren_relation_ibfk_2` FOREIGN KEY (`servicegroup_sg_id`) REFERENCES `cfg_servicegroups` (`sg_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_dependencies_servicegroupparents_relations`
--

DROP TABLE IF EXISTS `cfg_dependencies_servicegroupparents_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_dependencies_servicegroupparents_relations` (
  `dsgpr_id` int(11) NOT NULL AUTO_INCREMENT,
  `dependency_dep_id` int(11) DEFAULT NULL,
  `servicegroup_sg_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`dsgpr_id`),
  KEY `dependency_index` (`dependency_dep_id`),
  KEY `sg_index` (`servicegroup_sg_id`),
  CONSTRAINT `cfg_dependencies_servicegroupParent_relation_ibfk_1` FOREIGN KEY (`dependency_dep_id`) REFERENCES `cfg_dependencies` (`dep_id`) ON DELETE CASCADE,
  CONSTRAINT `dependency_servicegroupParent_relation_ibfk_2` FOREIGN KEY (`servicegroup_sg_id`) REFERENCES `cfg_servicegroups` (`sg_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_dependencies_serviceparents_relations`
--

DROP TABLE IF EXISTS `cfg_dependencies_serviceparents_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_dependencies_serviceparents_relations` (
  `dspr_id` int(11) NOT NULL AUTO_INCREMENT,
  `dependency_dep_id` int(11) DEFAULT NULL,
  `service_service_id` int(11) DEFAULT NULL,
  `host_host_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`dspr_id`),
  KEY `dependency_index` (`dependency_dep_id`),
  KEY `service_index` (`service_service_id`),
  KEY `host_index` (`host_host_id`),
  CONSTRAINT `cfg_dependencies_serviceParent_relation_ibfk_1` FOREIGN KEY (`dependency_dep_id`) REFERENCES `cfg_dependencies` (`dep_id`) ON DELETE CASCADE,
  CONSTRAINT `dependency_serviceParent_relation_ibfk_2` FOREIGN KEY (`service_service_id`) REFERENCES `cfg_services` (`service_id`) ON DELETE CASCADE,
  CONSTRAINT `dependency_serviceParent_relation_ibfk_3` FOREIGN KEY (`host_host_id`) REFERENCES `cfg_hosts` (`host_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_domains`
--

DROP TABLE IF EXISTS `cfg_domains`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_domains` (
  `domain_id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL,
  `description` varchar(255) DEFAULT NULL,
  `isroot` int(11) NOT NULL,
  `parent_id` int(11) DEFAULT NULL,
  `icon_id` int(10) unsigned DEFAULT NULL,
  PRIMARY KEY (`domain_id`),
  UNIQUE KEY `name_index` (`name`),
  KEY `FI_ains_ibfk_1` (`parent_id`),
  KEY `FI_ains_ibfk_2` (`icon_id`),
  CONSTRAINT `domains_ibfk_1` FOREIGN KEY (`parent_id`) REFERENCES `cfg_domains` (`domain_id`) ON DELETE CASCADE,
  CONSTRAINT `domains_ibfk_2` FOREIGN KEY (`icon_id`) REFERENCES `cfg_binaries` (`binary_id`) ON DELETE SET NULL
) ENGINE=InnoDB AUTO_INCREMENT=10 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_downtimes`
--

DROP TABLE IF EXISTS `cfg_downtimes`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_downtimes` (
  `dt_id` int(11) NOT NULL AUTO_INCREMENT,
  `dt_name` varchar(100) NOT NULL,
  `dt_description` varchar(255) DEFAULT NULL,
  `dt_activate` enum('0','1') DEFAULT '1',
  `organization_id` int(11) NOT NULL,
  PRIMARY KEY (`dt_id`),
  UNIQUE KEY `downtime_idx02` (`dt_name`),
  KEY `downtime_idx01` (`dt_id`,`dt_activate`),
  KEY `FI__downtimes_ibfk_1` (`organization_id`),
  CONSTRAINT `cfg_downtimes_ibfk_1` FOREIGN KEY (`organization_id`) REFERENCES `cfg_organizations` (`organization_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_downtimes_hostgroups_relations`
--

DROP TABLE IF EXISTS `cfg_downtimes_hostgroups_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_downtimes_hostgroups_relations` (
  `dt_id` int(11) NOT NULL,
  `hg_hg_id` int(11) NOT NULL,
  PRIMARY KEY (`dt_id`,`hg_hg_id`),
  KEY `downtime_hostgroup_relation_ibfk_1` (`hg_hg_id`),
  CONSTRAINT `cfg_downtime_hostgroup_relation_ibfk_1` FOREIGN KEY (`hg_hg_id`) REFERENCES `cfg_hostgroups` (`hg_id`) ON DELETE CASCADE,
  CONSTRAINT `downtime_hostgroup_relation_ibfk_2` FOREIGN KEY (`dt_id`) REFERENCES `cfg_downtimes` (`dt_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_downtimes_hosts_relations`
--

DROP TABLE IF EXISTS `cfg_downtimes_hosts_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_downtimes_hosts_relations` (
  `dt_id` int(11) NOT NULL,
  `host_host_id` int(11) NOT NULL,
  PRIMARY KEY (`dt_id`,`host_host_id`),
  KEY `downtime_host_relation_ibfk_1` (`host_host_id`),
  CONSTRAINT `downtime_host_relation_ibfk_1` FOREIGN KEY (`host_host_id`) REFERENCES `cfg_hosts` (`host_id`) ON DELETE CASCADE,
  CONSTRAINT `downtime_host_relation_ibfk_2` FOREIGN KEY (`dt_id`) REFERENCES `cfg_downtimes` (`dt_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_downtimes_periods`
--

DROP TABLE IF EXISTS `cfg_downtimes_periods`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_downtimes_periods` (
  `dt_id` int(11) NOT NULL,
  `dtp_start_time` time NOT NULL,
  `dtp_end_time` time NOT NULL,
  `dtp_day_of_week` varchar(15) DEFAULT NULL,
  `dtp_month_cycle` enum('first','last','all','none') DEFAULT 'all',
  `dtp_day_of_month` varchar(100) DEFAULT NULL,
  `dtp_fixed` enum('0','1') DEFAULT '1',
  `dtp_duration` int(11) DEFAULT NULL,
  `dtp_next_date` date DEFAULT NULL,
  `dtp_activate` enum('0','1') DEFAULT '1',
  `organization_id` int(11) NOT NULL,
  KEY `downtime_period_idx01` (`dt_id`,`dtp_activate`),
  KEY `FI_ntime_period_ibfk_2` (`organization_id`),
  CONSTRAINT `downtime_period_ibfk_1` FOREIGN KEY (`dt_id`) REFERENCES `cfg_downtimes` (`dt_id`) ON DELETE CASCADE,
  CONSTRAINT `downtime_period_ibfk_2` FOREIGN KEY (`organization_id`) REFERENCES `cfg_organizations` (`organization_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_downtimes_servicegroups_relations`
--

DROP TABLE IF EXISTS `cfg_downtimes_servicegroups_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_downtimes_servicegroups_relations` (
  `dt_id` int(11) NOT NULL,
  `sg_sg_id` int(11) NOT NULL,
  PRIMARY KEY (`dt_id`,`sg_sg_id`),
  KEY `downtime_servicegroup_relation_ibfk_1` (`sg_sg_id`),
  CONSTRAINT `downtime_servicegroup_relation_ibfk_1` FOREIGN KEY (`sg_sg_id`) REFERENCES `cfg_servicegroups` (`sg_id`) ON DELETE CASCADE,
  CONSTRAINT `downtime_servicegroup_relation_ibfk_2` FOREIGN KEY (`dt_id`) REFERENCES `cfg_downtimes` (`dt_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_downtimes_services_relations`
--

DROP TABLE IF EXISTS `cfg_downtimes_services_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_downtimes_services_relations` (
  `dt_id` int(11) NOT NULL,
  `host_host_id` int(11) NOT NULL,
  `service_service_id` int(11) NOT NULL,
  PRIMARY KEY (`dt_id`,`host_host_id`,`service_service_id`),
  KEY `downtime_service_relation_ibfk_1` (`service_service_id`),
  KEY `downtime_service_relation_ibfk_3` (`host_host_id`),
  CONSTRAINT `downtime_service_relation_ibfk_1` FOREIGN KEY (`service_service_id`) REFERENCES `cfg_services` (`service_id`) ON DELETE CASCADE,
  CONSTRAINT `downtime_service_relation_ibfk_2` FOREIGN KEY (`dt_id`) REFERENCES `cfg_downtimes` (`dt_id`) ON DELETE CASCADE,
  CONSTRAINT `downtime_service_relation_ibfk_3` FOREIGN KEY (`host_host_id`) REFERENCES `cfg_hosts` (`host_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_engine`
--

DROP TABLE IF EXISTS `cfg_engine`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_engine` (
  `poller_id` int(11) NOT NULL,
  `conf_dir` varchar(255) DEFAULT NULL,
  `log_dir` varchar(255) DEFAULT NULL,
  `var_lib_dir` varchar(255) DEFAULT NULL,
  `module_dir` varchar(255) DEFAULT NULL,
  `init_script` varchar(255) DEFAULT NULL,
  `enable_notifications` tinyint(1) DEFAULT NULL,
  `enable_event_handlers` tinyint(1) DEFAULT NULL,
  `external_command_buffer_slots` int(11) DEFAULT NULL,
  `command_check_interval` varchar(255) DEFAULT NULL,
  `command_file` varchar(255) DEFAULT NULL,
  `use_syslog` tinyint(1) DEFAULT NULL,
  `log_notifications` tinyint(1) DEFAULT NULL,
  `log_service_retries` tinyint(1) DEFAULT NULL,
  `log_host_retries` tinyint(1) DEFAULT NULL,
  `log_event_handlers` tinyint(1) DEFAULT NULL,
  `log_initial_states` tinyint(1) DEFAULT NULL,
  `log_external_commands` tinyint(1) DEFAULT NULL,
  `log_passive_checks` tinyint(1) DEFAULT NULL,
  `global_host_event_handler` int(11) DEFAULT NULL,
  `global_service_event_handler` int(11) DEFAULT NULL,
  `max_concurrent_checks` int(11) DEFAULT NULL,
  `max_service_check_spread` int(11) DEFAULT NULL,
  `max_host_check_spread` int(11) DEFAULT NULL,
  `check_result_reaper_frequency` int(11) DEFAULT NULL,
  `enable_flap_detection` tinyint(1) DEFAULT NULL,
  `low_service_flap_threshold` varchar(255) DEFAULT NULL,
  `high_service_flap_threshold` varchar(255) DEFAULT NULL,
  `low_host_flap_threshold` varchar(255) DEFAULT NULL,
  `high_host_flap_threshold` varchar(255) DEFAULT NULL,
  `service_check_timeout` int(11) DEFAULT NULL,
  `host_check_timeout` int(11) DEFAULT NULL,
  `event_handler_timeout` int(11) DEFAULT NULL,
  `notification_timeout` int(11) DEFAULT NULL,
  `ocsp_timeout` int(11) DEFAULT NULL,
  `ochp_timeout` int(11) DEFAULT NULL,
  `ocsp_command` int(11) DEFAULT NULL,
  `ochp_command` int(11) DEFAULT NULL,
  `check_service_freshness` tinyint(1) DEFAULT NULL,
  `freshness_check_interval` int(11) DEFAULT NULL,
  `check_host_freshness` tinyint(1) DEFAULT NULL,
  `date_format` varchar(255) DEFAULT NULL,
  `enable_predictive_host_dependency_checks` tinyint(1) DEFAULT NULL,
  `enable_predictive_service_dependency_checks` tinyint(1) DEFAULT NULL,
  `debug_level` int(11) DEFAULT NULL,
  `debug_verbosity` tinyint(1) DEFAULT NULL,
  `max_debug_file_size` int(11) DEFAULT NULL,
  PRIMARY KEY (`poller_id`),
  KEY `cmd1_index` (`global_host_event_handler`),
  KEY `cmd2_index` (`global_service_event_handler`),
  KEY `cmd3_index` (`ocsp_command`),
  KEY `cmd4_index` (`ochp_command`),
  KEY `poller_id` (`poller_id`),
  CONSTRAINT `cfg_engine_ibfk_gheh` FOREIGN KEY (`global_host_event_handler`) REFERENCES `cfg_commands` (`command_id`) ON DELETE SET NULL,
  CONSTRAINT `cfg_engine_ibfk_gseh` FOREIGN KEY (`global_service_event_handler`) REFERENCES `cfg_commands` (`command_id`) ON DELETE SET NULL,
  CONSTRAINT `cfg_engine_ibfk_ochpc` FOREIGN KEY (`ochp_command`) REFERENCES `cfg_commands` (`command_id`) ON DELETE SET NULL,
  CONSTRAINT `cfg_engine_ibfk_ocspc` FOREIGN KEY (`ocsp_command`) REFERENCES `cfg_commands` (`command_id`) ON DELETE SET NULL,
  CONSTRAINT `cfg_engine_ibfk_poller_id` FOREIGN KEY (`poller_id`) REFERENCES `cfg_pollers` (`poller_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_engine_broker_module`
--

DROP TABLE IF EXISTS `cfg_engine_broker_module`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_engine_broker_module` (
  `bk_mod_id` int(11) NOT NULL AUTO_INCREMENT,
  `poller_id` int(11) DEFAULT NULL,
  `broker_module` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`bk_mod_id`),
  KEY `fk_engine_cfg` (`poller_id`),
  CONSTRAINT `fk_engine_cfg` FOREIGN KEY (`poller_id`) REFERENCES `cfg_engine` (`poller_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_engine_macros`
--

DROP TABLE IF EXISTS `cfg_engine_macros`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_engine_macros` (
  `macro_id` int(11) NOT NULL AUTO_INCREMENT,
  `macro_name` varchar(255) DEFAULT NULL,
  `organization_id` int(11) NOT NULL,
  PRIMARY KEY (`macro_id`),
  KEY `FI_ine_macros_ibfk_1` (`organization_id`),
  CONSTRAINT `engine_macros_ibfk_1` FOREIGN KEY (`organization_id`) REFERENCES `cfg_organizations` (`organization_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_environments`
--

DROP TABLE IF EXISTS `cfg_environments`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_environments` (
  `environment_id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL,
  `description` varchar(255) DEFAULT NULL,
  `level` int(11) NOT NULL,
  `organization_id` int(11) NOT NULL,
  `icon_id` int(10) unsigned DEFAULT NULL,
  PRIMARY KEY (`environment_id`),
  UNIQUE KEY `name_index` (`name`),
  KEY `FI_ironments_ibfk_1` (`organization_id`),
  KEY `FI_ironments_ibfk_2` (`icon_id`),
  CONSTRAINT `environments_ibfk_1` FOREIGN KEY (`organization_id`) REFERENCES `cfg_organizations` (`organization_id`) ON DELETE CASCADE,
  CONSTRAINT `environments_ibfk_2` FOREIGN KEY (`icon_id`) REFERENCES `cfg_binaries` (`binary_id`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_escalations`
--

DROP TABLE IF EXISTS `cfg_escalations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_escalations` (
  `esc_id` int(11) NOT NULL AUTO_INCREMENT,
  `esc_name` varchar(255) DEFAULT NULL,
  `esc_alias` varchar(255) DEFAULT NULL,
  `first_notification` int(11) DEFAULT NULL,
  `last_notification` int(11) DEFAULT NULL,
  `notification_interval` int(11) DEFAULT NULL,
  `escalation_period` int(11) DEFAULT NULL,
  `escalation_options1` varchar(255) DEFAULT NULL,
  `escalation_options2` varchar(255) DEFAULT NULL,
  `esc_comment` text,
  `organization_id` int(11) NOT NULL,
  PRIMARY KEY (`esc_id`),
  KEY `period_index` (`escalation_period`),
  KEY `FI_alations_ibfk_2` (`organization_id`),
  CONSTRAINT `escalation_ibfk_1` FOREIGN KEY (`escalation_period`) REFERENCES `cfg_timeperiods` (`tp_id`) ON DELETE SET NULL,
  CONSTRAINT `escalations_ibfk_2` FOREIGN KEY (`organization_id`) REFERENCES `cfg_organizations` (`organization_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_escalations_hostgroups_relations`
--

DROP TABLE IF EXISTS `cfg_escalations_hostgroups_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_escalations_hostgroups_relations` (
  `ehgr_id` int(11) NOT NULL AUTO_INCREMENT,
  `escalation_esc_id` int(11) DEFAULT NULL,
  `hostgroup_hg_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`ehgr_id`),
  KEY `escalation_index` (`escalation_esc_id`),
  KEY `hg_index` (`hostgroup_hg_id`),
  CONSTRAINT `cfg_escalation_hostgroup_relation_ibfk_2` FOREIGN KEY (`hostgroup_hg_id`) REFERENCES `cfg_hostgroups` (`hg_id`) ON DELETE CASCADE,
  CONSTRAINT `escalation_hostgroup_relation_ibfk_1` FOREIGN KEY (`escalation_esc_id`) REFERENCES `cfg_escalations` (`esc_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_escalations_hosts_relations`
--

DROP TABLE IF EXISTS `cfg_escalations_hosts_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_escalations_hosts_relations` (
  `ehr_id` int(11) NOT NULL AUTO_INCREMENT,
  `escalation_esc_id` int(11) DEFAULT NULL,
  `host_host_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`ehr_id`),
  KEY `escalation_index` (`escalation_esc_id`),
  KEY `host_index` (`host_host_id`),
  CONSTRAINT `escalation_host_relation_ibfk_1` FOREIGN KEY (`escalation_esc_id`) REFERENCES `cfg_escalations` (`esc_id`) ON DELETE CASCADE,
  CONSTRAINT `escalation_host_relation_ibfk_2` FOREIGN KEY (`host_host_id`) REFERENCES `cfg_hosts` (`host_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_escalations_metaservices_relations`
--

DROP TABLE IF EXISTS `cfg_escalations_metaservices_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_escalations_metaservices_relations` (
  `emsr_id` int(11) NOT NULL AUTO_INCREMENT,
  `escalation_esc_id` int(11) DEFAULT NULL,
  `meta_service_meta_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`emsr_id`),
  KEY `escalation_index` (`escalation_esc_id`),
  KEY `meta_service_index` (`meta_service_meta_id`),
  CONSTRAINT `cfg_escalations_metaservices_relations_ibfk_1` FOREIGN KEY (`escalation_esc_id`) REFERENCES `cfg_escalations` (`esc_id`) ON DELETE CASCADE,
  CONSTRAINT `cfg_escalations_metaservices_relations_ibfk_2` FOREIGN KEY (`meta_service_meta_id`) REFERENCES `cfg_meta_services` (`meta_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_escalations_servicegroups_relations`
--

DROP TABLE IF EXISTS `cfg_escalations_servicegroups_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_escalations_servicegroups_relations` (
  `esgr_id` int(11) NOT NULL AUTO_INCREMENT,
  `escalation_esc_id` int(11) DEFAULT NULL,
  `servicegroup_sg_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`esgr_id`),
  KEY `escalation_index` (`escalation_esc_id`),
  KEY `sg_index` (`servicegroup_sg_id`),
  CONSTRAINT `escalation_servicegroup_relation_ibfk_1` FOREIGN KEY (`escalation_esc_id`) REFERENCES `cfg_escalations` (`esc_id`) ON DELETE CASCADE,
  CONSTRAINT `escalation_servicegroup_relation_ibfk_2` FOREIGN KEY (`servicegroup_sg_id`) REFERENCES `cfg_servicegroups` (`sg_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_escalations_services_relations`
--

DROP TABLE IF EXISTS `cfg_escalations_services_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_escalations_services_relations` (
  `esr_id` int(11) NOT NULL AUTO_INCREMENT,
  `escalation_esc_id` int(11) DEFAULT NULL,
  `service_service_id` int(11) DEFAULT NULL,
  `host_host_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`esr_id`),
  KEY `escalation_index` (`escalation_esc_id`),
  KEY `service_index` (`service_service_id`),
  KEY `host_index` (`host_host_id`),
  CONSTRAINT `escalation_service_relation_ibfk_1` FOREIGN KEY (`escalation_esc_id`) REFERENCES `cfg_escalations` (`esc_id`) ON DELETE CASCADE,
  CONSTRAINT `escalation_service_relation_ibfk_2` FOREIGN KEY (`service_service_id`) REFERENCES `cfg_services` (`service_id`) ON DELETE CASCADE,
  CONSTRAINT `escalation_service_relation_ibfk_3` FOREIGN KEY (`host_host_id`) REFERENCES `cfg_hosts` (`host_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_forms`
--

DROP TABLE IF EXISTS `cfg_forms`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_forms` (
  `form_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL,
  `route` varchar(255) NOT NULL,
  `redirect` char(1) NOT NULL DEFAULT '0',
  `redirect_route` varchar(255) DEFAULT NULL,
  `module_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`form_id`,`module_id`),
  UNIQUE KEY `route_UNIQUE` (`route`),
  KEY `FI_form_module_1` (`module_id`),
  CONSTRAINT `fk_form_module_1` FOREIGN KEY (`module_id`) REFERENCES `cfg_modules` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=30 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_forms_blocks`
--

DROP TABLE IF EXISTS `cfg_forms_blocks`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_forms_blocks` (
  `block_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL,
  `rank` int(10) unsigned NOT NULL,
  `section_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`block_id`,`section_id`),
  KEY `fk_form_block_1_idx` (`section_id`),
  CONSTRAINT `fk_form_block_1` FOREIGN KEY (`section_id`) REFERENCES `cfg_forms_sections` (`section_id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=132 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_forms_blocks_fields_relations`
--

DROP TABLE IF EXISTS `cfg_forms_blocks_fields_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_forms_blocks_fields_relations` (
  `block_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `field_id` int(10) unsigned NOT NULL,
  `rank` int(10) unsigned NOT NULL,
  `product_version` varchar(20) NOT NULL DEFAULT '',
  PRIMARY KEY (`block_id`,`field_id`,`product_version`),
  KEY `fk_form_block_field_relation_1_idx` (`block_id`),
  KEY `fk_form_block_field_relation_2_idx` (`field_id`),
  CONSTRAINT `fk_form_block_field_relation_1` FOREIGN KEY (`block_id`) REFERENCES `cfg_forms_blocks` (`block_id`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `fk_form_block_field_relation_2` FOREIGN KEY (`field_id`) REFERENCES `cfg_forms_fields` (`field_id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=132 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_forms_fields`
--

DROP TABLE IF EXISTS `cfg_forms_fields`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_forms_fields` (
  `field_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL,
  `label` varchar(45) NOT NULL,
  `default_value` varchar(45) NOT NULL,
  `attributes` varchar(255) DEFAULT NULL,
  `advanced` char(1) NOT NULL DEFAULT '0',
  `mandatory` char(1) NOT NULL DEFAULT '0',
  `type` varchar(45) NOT NULL,
  `help` text,
  `help_url` varchar(255) DEFAULT NULL,
  `module_id` int(10) unsigned NOT NULL,
  `parent_field` varchar(45) DEFAULT NULL,
  `child_actions` varchar(45) DEFAULT NULL,
  PRIMARY KEY (`field_id`),
  KEY `fk_field_module1_idx` (`module_id`),
  CONSTRAINT `fk_form_field_1` FOREIGN KEY (`module_id`) REFERENCES `cfg_modules` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=325 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_forms_fields_validators_relations`
--

DROP TABLE IF EXISTS `cfg_forms_fields_validators_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_forms_fields_validators_relations` (
  `field_id` int(10) unsigned NOT NULL,
  `validator_id` int(10) unsigned NOT NULL,
  `params` varchar(255) NOT NULL,
  `client_side_event` varchar(45) DEFAULT NULL,
  PRIMARY KEY (`field_id`,`validator_id`),
  KEY `fk_new_table_1_idx` (`field_id`),
  KEY `fk_form_field_validator_relation_2_idx` (`validator_id`),
  CONSTRAINT `fk_form_field_validator_relation_1` FOREIGN KEY (`field_id`) REFERENCES `cfg_forms_fields` (`field_id`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `fk_form_field_validator_relation_2` FOREIGN KEY (`validator_id`) REFERENCES `cfg_forms_validators` (`validator_id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_forms_massive_change`
--

DROP TABLE IF EXISTS `cfg_forms_massive_change`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_forms_massive_change` (
  `massive_change_id` int(10) unsigned NOT NULL,
  `name` varchar(45) NOT NULL,
  `route` varchar(45) NOT NULL,
  PRIMARY KEY (`massive_change_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_forms_massive_change_fields_relations`
--

DROP TABLE IF EXISTS `cfg_forms_massive_change_fields_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_forms_massive_change_fields_relations` (
  `massive_change_id` int(10) unsigned NOT NULL,
  `field_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`massive_change_id`,`field_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_forms_sections`
--

DROP TABLE IF EXISTS `cfg_forms_sections`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_forms_sections` (
  `section_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL,
  `rank` int(10) unsigned NOT NULL,
  `form_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`section_id`,`form_id`),
  KEY `fk_section_form1_idx` (`form_id`),
  CONSTRAINT `fk_form_section_1` FOREIGN KEY (`form_id`) REFERENCES `cfg_forms` (`form_id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=61 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_forms_steps`
--

DROP TABLE IF EXISTS `cfg_forms_steps`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_forms_steps` (
  `step_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL,
  `wizard_id` int(10) unsigned NOT NULL,
  `rank` int(10) unsigned NOT NULL,
  PRIMARY KEY (`step_id`,`wizard_id`),
  KEY `fk_step_wizard_idx` (`wizard_id`)
) ENGINE=InnoDB AUTO_INCREMENT=43 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_forms_steps_fields_relations`
--

DROP TABLE IF EXISTS `cfg_forms_steps_fields_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_forms_steps_fields_relations` (
  `step_id` int(10) unsigned NOT NULL,
  `field_id` int(10) unsigned NOT NULL,
  `rank` int(10) unsigned NOT NULL,
  PRIMARY KEY (`step_id`,`field_id`),
  KEY `fk_step_has_field_field1_idx` (`field_id`),
  KEY `fk_step_has_field_step1_idx` (`step_id`),
  CONSTRAINT `fk_form_step_field_relation_1` FOREIGN KEY (`step_id`) REFERENCES `cfg_forms_steps` (`step_id`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `fk_form_step_field_relation_2` FOREIGN KEY (`field_id`) REFERENCES `cfg_forms_fields` (`field_id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_forms_validators`
--

DROP TABLE IF EXISTS `cfg_forms_validators`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_forms_validators` (
  `validator_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL,
  `route` varchar(255) NOT NULL,
  PRIMARY KEY (`validator_id`)
) ENGINE=InnoDB AUTO_INCREMENT=11 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_forms_wizards`
--

DROP TABLE IF EXISTS `cfg_forms_wizards`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_forms_wizards` (
  `wizard_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL,
  `route` varchar(255) NOT NULL,
  `module_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`wizard_id`,`module_id`),
  KEY `FI_wizard_module_1` (`module_id`),
  CONSTRAINT `fk_wizard_module_1` FOREIGN KEY (`module_id`) REFERENCES `cfg_modules` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=28 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_hooks`
--

DROP TABLE IF EXISTS `cfg_hooks`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_hooks` (
  `hook_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `hook_name` varchar(255) DEFAULT NULL,
  `hook_description` varchar(255) DEFAULT NULL,
  `hook_type` tinyint(1) DEFAULT '0',
  PRIMARY KEY (`hook_id`)
) ENGINE=InnoDB AUTO_INCREMENT=11 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_hostcategories`
--

DROP TABLE IF EXISTS `cfg_hostcategories`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_hostcategories` (
  `hc_id` int(11) NOT NULL AUTO_INCREMENT,
  `hc_name` varchar(200) DEFAULT NULL,
  `hc_alias` varchar(200) DEFAULT NULL,
  `level` tinyint(5) DEFAULT NULL,
  `icon_id` int(10) unsigned DEFAULT NULL,
  `hc_comment` text,
  `hc_activate` enum('0','1') NOT NULL DEFAULT '1',
  `organization_id` int(11) NOT NULL,
  PRIMARY KEY (`hc_id`),
  UNIQUE KEY `hc_unique_01` (`hc_name`),
  KEY `name_index` (`hc_name`),
  KEY `alias_index` (`hc_alias`),
  KEY `FI_tcategory_image_relation_ibfk_1` (`icon_id`),
  KEY `FI_tcategories_ibfk_1` (`organization_id`),
  CONSTRAINT `hostcategories_ibfk_1` FOREIGN KEY (`organization_id`) REFERENCES `cfg_organizations` (`organization_id`) ON DELETE CASCADE,
  CONSTRAINT `hostcategory_image_relation_ibfk_1` FOREIGN KEY (`icon_id`) REFERENCES `cfg_binaries` (`binary_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_hostcategories_relations`
--

DROP TABLE IF EXISTS `cfg_hostcategories_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_hostcategories_relations` (
  `hcr_id` int(11) NOT NULL AUTO_INCREMENT,
  `hostcategories_hc_id` int(11) DEFAULT NULL,
  `host_host_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`hcr_id`),
  KEY `hostcategories_index` (`hostcategories_hc_id`),
  KEY `host_index` (`host_host_id`),
  CONSTRAINT `hostcategories_relation_ibfk_1` FOREIGN KEY (`hostcategories_hc_id`) REFERENCES `cfg_hostcategories` (`hc_id`) ON DELETE CASCADE,
  CONSTRAINT `hostcategories_relation_ibfk_2` FOREIGN KEY (`host_host_id`) REFERENCES `cfg_hosts` (`host_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_hostgroups`
--

DROP TABLE IF EXISTS `cfg_hostgroups`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_hostgroups` (
  `hg_id` int(11) NOT NULL AUTO_INCREMENT,
  `hg_name` varchar(200) DEFAULT NULL,
  `hg_alias` varchar(200) DEFAULT NULL,
  `hg_notes` varchar(255) DEFAULT NULL,
  `hg_notes_url` varchar(255) DEFAULT NULL,
  `hg_action_url` varchar(255) DEFAULT NULL,
  `hg_icon_image` int(11) DEFAULT NULL,
  `hg_map_icon_image` int(11) DEFAULT NULL,
  `hg_rrd_retention` int(11) DEFAULT NULL,
  `hg_comment` text,
  `hg_activate` enum('0','1') NOT NULL DEFAULT '1',
  `organization_id` int(11) NOT NULL,
  PRIMARY KEY (`hg_id`),
  UNIQUE KEY `hg_unique_01` (`hg_name`),
  KEY `name_index` (`hg_name`),
  KEY `alias_index` (`hg_alias`),
  KEY `FI__hostgroups_ibfk_1` (`organization_id`),
  CONSTRAINT `cfg_hostgroups_ibfk_1` FOREIGN KEY (`organization_id`) REFERENCES `cfg_organizations` (`organization_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_hostgroups_hostgroups_relations`
--

DROP TABLE IF EXISTS `cfg_hostgroups_hostgroups_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_hostgroups_hostgroups_relations` (
  `hgr_id` int(11) NOT NULL AUTO_INCREMENT,
  `hg_parent_id` int(11) DEFAULT NULL,
  `hg_children_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`hgr_id`),
  KEY `hg_parent_id` (`hg_parent_id`),
  KEY `hg_children_id` (`hg_children_id`),
  CONSTRAINT `cfg_hostgroup_hg_relation_ibfk_1` FOREIGN KEY (`hg_parent_id`) REFERENCES `cfg_hostgroups` (`hg_id`) ON DELETE CASCADE,
  CONSTRAINT `cfg_hostgroup_hg_relation_ibfk_2` FOREIGN KEY (`hg_children_id`) REFERENCES `cfg_hostgroups` (`hg_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_hostgroups_relations`
--

DROP TABLE IF EXISTS `cfg_hostgroups_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_hostgroups_relations` (
  `hgr_id` int(11) NOT NULL AUTO_INCREMENT,
  `hostgroup_hg_id` int(11) DEFAULT NULL,
  `host_host_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`hgr_id`),
  KEY `hostgroup_index` (`hostgroup_hg_id`),
  KEY `host_index` (`host_host_id`),
  CONSTRAINT `cfg_hostgroup_relation_ibfk_1` FOREIGN KEY (`hostgroup_hg_id`) REFERENCES `cfg_hostgroups` (`hg_id`) ON DELETE CASCADE,
  CONSTRAINT `hostgroup_relation_ibfk_2` FOREIGN KEY (`host_host_id`) REFERENCES `cfg_hosts` (`host_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_hosts`
--

DROP TABLE IF EXISTS `cfg_hosts`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_hosts` (
  `host_id` int(11) NOT NULL AUTO_INCREMENT,
  `host_template_model_htm_id` int(11) DEFAULT NULL,
  `command_command_id` int(11) DEFAULT NULL,
  `command_command_id_arg1` text,
  `timeperiod_tp_id` int(11) DEFAULT NULL,
  `timeperiod_tp_id2` int(11) DEFAULT NULL,
  `command_command_id2` int(11) DEFAULT NULL,
  `command_command_id_arg2` text,
  `host_name` varchar(200) DEFAULT NULL,
  `host_alias` varchar(200) DEFAULT NULL,
  `host_address` varchar(255) DEFAULT NULL,
  `display_name` varchar(255) DEFAULT NULL,
  `host_max_check_attempts` int(11) DEFAULT NULL,
  `host_check_interval` int(11) DEFAULT NULL,
  `host_retry_check_interval` int(11) DEFAULT NULL,
  `host_active_checks_enabled` enum('0','1','2') DEFAULT NULL,
  `host_passive_checks_enabled` enum('0','1','2') DEFAULT NULL,
  `host_checks_enabled` enum('0','1','2') DEFAULT NULL,
  `initial_state` enum('o','d','u') DEFAULT NULL,
  `host_obsess_over_host` enum('0','1','2') DEFAULT NULL,
  `host_check_freshness` enum('0','1','2') DEFAULT NULL,
  `host_freshness_threshold` int(11) DEFAULT NULL,
  `host_event_handler_enabled` enum('0','1','2') DEFAULT NULL,
  `host_low_flap_threshold` int(11) DEFAULT NULL,
  `host_high_flap_threshold` int(11) DEFAULT NULL,
  `host_flap_detection_enabled` enum('0','1','2') DEFAULT NULL,
  `flap_detection_options` varchar(255) DEFAULT NULL,
  `host_process_perf_data` enum('0','1','2') DEFAULT NULL,
  `host_retain_status_information` enum('0','1','2') DEFAULT NULL,
  `host_retain_nonstatus_information` enum('0','1','2') DEFAULT NULL,
  `host_notification_interval` int(11) DEFAULT NULL,
  `host_notification_options` varchar(200) DEFAULT NULL,
  `host_notifications_enabled` enum('0','1','2') DEFAULT NULL,
  `contact_additive_inheritance` tinyint(1) DEFAULT '0',
  `cg_additive_inheritance` tinyint(1) DEFAULT '0',
  `host_first_notification_delay` int(11) DEFAULT NULL,
  `host_stalking_options` varchar(200) DEFAULT NULL,
  `host_snmp_community` varchar(255) DEFAULT NULL,
  `host_snmp_version` varchar(255) DEFAULT NULL,
  `host_location` int(11) DEFAULT '0',
  `host_comment` text,
  `host_register` enum('0','1','2','3') DEFAULT NULL,
  `host_activate` enum('0','1','2') DEFAULT '1',
  `organization_id` int(11) NOT NULL,
  `environment_id` int(11) DEFAULT NULL,
  `poller_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`host_id`),
  UNIQUE KEY `name_index` (`host_name`,`organization_id`),
  KEY `htm_index` (`host_template_model_htm_id`),
  KEY `cmd1_index` (`command_command_id`),
  KEY `cmd2_index` (`command_command_id2`),
  KEY `tp1_index` (`timeperiod_tp_id`),
  KEY `tp2_index` (`timeperiod_tp_id2`),
  KEY `host_id_register` (`host_id`,`host_register`),
  KEY `alias_index` (`host_alias`),
  KEY `host_register` (`host_register`),
  KEY `FI_t_ibfk_5` (`organization_id`),
  KEY `FI_t_ibfk_6` (`environment_id`),
  KEY `FI_t_ibfk_7` (`poller_id`),
  CONSTRAINT `host_ibfk_1` FOREIGN KEY (`command_command_id`) REFERENCES `cfg_commands` (`command_id`) ON DELETE SET NULL,
  CONSTRAINT `host_ibfk_2` FOREIGN KEY (`command_command_id2`) REFERENCES `cfg_commands` (`command_id`) ON DELETE SET NULL,
  CONSTRAINT `host_ibfk_3` FOREIGN KEY (`timeperiod_tp_id`) REFERENCES `cfg_timeperiods` (`tp_id`) ON DELETE SET NULL,
  CONSTRAINT `host_ibfk_4` FOREIGN KEY (`timeperiod_tp_id2`) REFERENCES `cfg_timeperiods` (`tp_id`) ON DELETE SET NULL,
  CONSTRAINT `host_ibfk_5` FOREIGN KEY (`organization_id`) REFERENCES `cfg_organizations` (`organization_id`) ON DELETE CASCADE,
  CONSTRAINT `host_ibfk_6` FOREIGN KEY (`environment_id`) REFERENCES `cfg_environments` (`environment_id`) ON DELETE CASCADE,
  CONSTRAINT `host_ibfk_7` FOREIGN KEY (`poller_id`) REFERENCES `cfg_pollers` (`poller_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_hosts_checkcmd_args_relations`
--

DROP TABLE IF EXISTS `cfg_hosts_checkcmd_args_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_hosts_checkcmd_args_relations` (
  `host_id` int(11) NOT NULL,
  `arg_number` tinyint(3) NOT NULL,
  `arg_value` varchar(255) DEFAULT NULL,
  KEY `fk_host_checkcmd_args_relations` (`host_id`),
  CONSTRAINT `fk_host_checkcmd_args_relations` FOREIGN KEY (`host_id`) REFERENCES `cfg_hosts` (`host_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_hosts_hostparents_relations`
--

DROP TABLE IF EXISTS `cfg_hosts_hostparents_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_hosts_hostparents_relations` (
  `hhr_id` int(11) NOT NULL AUTO_INCREMENT,
  `host_parent_hp_id` int(11) DEFAULT NULL,
  `host_host_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`hhr_id`),
  KEY `host1_index` (`host_parent_hp_id`),
  KEY `host2_index` (`host_host_id`),
  CONSTRAINT `host_hostparent_relation_ibfk_1` FOREIGN KEY (`host_parent_hp_id`) REFERENCES `cfg_hosts` (`host_id`) ON DELETE CASCADE,
  CONSTRAINT `host_hostparent_relation_ibfk_2` FOREIGN KEY (`host_host_id`) REFERENCES `cfg_hosts` (`host_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_hosts_images_relations`
--

DROP TABLE IF EXISTS `cfg_hosts_images_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_hosts_images_relations` (
  `host_id` int(11) NOT NULL,
  `binary_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`host_id`,`binary_id`),
  KEY `FI_t_image_relation_ibfk_2` (`binary_id`),
  CONSTRAINT `host_image_relation_ibfk_1` FOREIGN KEY (`host_id`) REFERENCES `cfg_hosts` (`host_id`) ON DELETE CASCADE,
  CONSTRAINT `host_image_relation_ibfk_2` FOREIGN KEY (`binary_id`) REFERENCES `cfg_binaries` (`binary_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_hosts_services_relations`
--

DROP TABLE IF EXISTS `cfg_hosts_services_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_hosts_services_relations` (
  `hsr_id` int(11) NOT NULL AUTO_INCREMENT,
  `hostgroup_hg_id` int(11) DEFAULT NULL,
  `host_host_id` int(11) DEFAULT NULL,
  `servicegroup_sg_id` int(11) DEFAULT NULL,
  `service_service_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`hsr_id`),
  KEY `hostgroup_index` (`hostgroup_hg_id`),
  KEY `host_index` (`host_host_id`),
  KEY `servicegroup_index` (`servicegroup_sg_id`),
  KEY `service_index` (`service_service_id`),
  KEY `host_service_index` (`host_host_id`,`service_service_id`),
  CONSTRAINT `cfg_host_service_relation_ibfk_1` FOREIGN KEY (`hostgroup_hg_id`) REFERENCES `cfg_hostgroups` (`hg_id`) ON DELETE CASCADE,
  CONSTRAINT `host_service_relation_ibfk_2` FOREIGN KEY (`host_host_id`) REFERENCES `cfg_hosts` (`host_id`) ON DELETE CASCADE,
  CONSTRAINT `host_service_relation_ibfk_3` FOREIGN KEY (`servicegroup_sg_id`) REFERENCES `cfg_servicegroups` (`sg_id`) ON DELETE CASCADE,
  CONSTRAINT `host_service_relation_ibfk_4` FOREIGN KEY (`service_service_id`) REFERENCES `cfg_services` (`service_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_hosts_templates_relations`
--

DROP TABLE IF EXISTS `cfg_hosts_templates_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_hosts_templates_relations` (
  `host_host_id` int(11) NOT NULL DEFAULT '0',
  `host_tpl_id` int(11) NOT NULL DEFAULT '0',
  `order` int(11) DEFAULT NULL,
  PRIMARY KEY (`host_host_id`,`host_tpl_id`),
  KEY `host_tpl_id` (`host_tpl_id`),
  CONSTRAINT `host_template_relation_ibfk_1` FOREIGN KEY (`host_host_id`) REFERENCES `cfg_hosts` (`host_id`) ON DELETE CASCADE,
  CONSTRAINT `host_template_relation_ibfk_2` FOREIGN KEY (`host_tpl_id`) REFERENCES `cfg_hosts` (`host_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_informations`
--

DROP TABLE IF EXISTS `cfg_informations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_informations` (
  `key` varchar(25) DEFAULT NULL,
  `value` varchar(25) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_languages`
--

DROP TABLE IF EXISTS `cfg_languages`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_languages` (
  `language_id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(200) NOT NULL,
  `description` varchar(200) DEFAULT NULL,
  PRIMARY KEY (`language_id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_menus`
--

DROP TABLE IF EXISTS `cfg_menus`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_menus` (
  `menu_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL,
  `short_name` varchar(255) NOT NULL,
  `parent_id` int(10) unsigned DEFAULT NULL,
  `url` varchar(255) DEFAULT NULL,
  `icon_class` varchar(100) DEFAULT NULL,
  `icon` varchar(255) DEFAULT NULL,
  `bgcolor` varchar(55) DEFAULT NULL,
  `module_id` int(10) unsigned NOT NULL,
  `menu_order` tinyint(5) DEFAULT '0',
  `menu_block` varchar(10) NOT NULL DEFAULT 'submenu',
  PRIMARY KEY (`menu_id`),
  KEY `parent_id` (`parent_id`),
  KEY `fk_menus_1_idx` (`module_id`),
  CONSTRAINT `fk_menu_parent_id` FOREIGN KEY (`parent_id`) REFERENCES `cfg_menus` (`menu_id`) ON DELETE SET NULL,
  CONSTRAINT `fk_menus_1` FOREIGN KEY (`module_id`) REFERENCES `cfg_modules` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=50 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_meta_services`
--

DROP TABLE IF EXISTS `cfg_meta_services`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_meta_services` (
  `meta_id` int(11) NOT NULL AUTO_INCREMENT,
  `meta_name` varchar(254) DEFAULT NULL,
  `meta_display` varchar(254) DEFAULT NULL,
  `check_period` int(11) DEFAULT NULL,
  `max_check_attempts` int(11) DEFAULT NULL,
  `normal_check_interval` int(11) DEFAULT NULL,
  `retry_check_interval` int(11) DEFAULT NULL,
  `notification_interval` int(11) DEFAULT NULL,
  `notification_period` int(11) DEFAULT NULL,
  `notification_options` varchar(255) DEFAULT NULL,
  `notifications_enabled` enum('0','1','2') DEFAULT NULL,
  `calcul_type` enum('SOM','AVE','MIN','MAX') DEFAULT NULL,
  `data_source_type` tinyint(3) NOT NULL DEFAULT '0',
  `meta_select_mode` enum('1','2') DEFAULT '1',
  `regexp_str` varchar(254) DEFAULT NULL,
  `metric` varchar(255) DEFAULT NULL,
  `warning` varchar(254) DEFAULT NULL,
  `critical` varchar(254) DEFAULT NULL,
  `meta_comment` text,
  `meta_activate` enum('0','1') DEFAULT NULL,
  `organization_id` int(11) NOT NULL,
  `value` FLOAT DEFAULT NULL,
  PRIMARY KEY (`meta_id`),
  KEY `name_index` (`meta_name`),
  KEY `check_period_index` (`check_period`),
  KEY `notification_period_index` (`notification_period`),
  KEY `FI_a_service_ibfk_3` (`organization_id`),
  CONSTRAINT `meta_service_ibfk_1` FOREIGN KEY (`check_period`) REFERENCES `cfg_timeperiods` (`tp_id`) ON DELETE SET NULL,
  CONSTRAINT `meta_service_ibfk_2` FOREIGN KEY (`notification_period`) REFERENCES `cfg_timeperiods` (`tp_id`) ON DELETE SET NULL,
  CONSTRAINT `meta_service_ibfk_3` FOREIGN KEY (`organization_id`) REFERENCES `cfg_organizations` (`organization_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_meta_services_relations`
--

DROP TABLE IF EXISTS `cfg_meta_services_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_meta_services_relations` (
  `msr_id` int(11) NOT NULL AUTO_INCREMENT,
  `meta_id` int(11) DEFAULT NULL,
  `host_id` int(11) DEFAULT NULL,
  `metric_id` int(11) DEFAULT NULL,
  `msr_comment` text,
  `activate` enum('0','1') DEFAULT NULL,
  PRIMARY KEY (`msr_id`),
  KEY `meta_index` (`meta_id`),
  KEY `metric_index` (`metric_id`),
  KEY `host_index` (`host_id`),
  CONSTRAINT `cfg_meta_services_relations_ibfk_1` FOREIGN KEY (`host_id`) REFERENCES `cfg_hosts` (`host_id`) ON DELETE CASCADE,
  CONSTRAINT `cfg_meta_services_relations_ibfk_2` FOREIGN KEY (`meta_id`) REFERENCES `cfg_meta_services` (`meta_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_modules`
--

DROP TABLE IF EXISTS `cfg_modules`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_modules` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL,
  `alias` varchar(45) DEFAULT NULL,
  `description` varchar(45) DEFAULT NULL,
  `version` varchar(45) NOT NULL,
  `author` varchar(255) NOT NULL,
  `isactivated` char(1) NOT NULL DEFAULT '0',
  `isinstalled` char(1) DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_modules_hooks`
--

DROP TABLE IF EXISTS `cfg_modules_hooks`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_modules_hooks` (
  `module_id` int(10) unsigned NOT NULL,
  `hook_id` int(10) unsigned NOT NULL,
  `module_hook_name` varchar(255) NOT NULL,
  `module_hook_description` varchar(255) NOT NULL,
  PRIMARY KEY (`module_id`,`hook_id`),
  KEY `fk_hook_id` (`hook_id`),
  CONSTRAINT `fk_hook_id` FOREIGN KEY (`hook_id`) REFERENCES `cfg_hooks` (`hook_id`) ON DELETE CASCADE,
  CONSTRAINT `fk_module_id` FOREIGN KEY (`module_id`) REFERENCES `cfg_modules` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_nodes`
--

DROP TABLE IF EXISTS `cfg_nodes`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_nodes` (
  `node_id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL,
  `ip_address` varchar(255) NOT NULL,
  `enable` int(1) NOT NULL DEFAULT '1',
  `multiple_poller` int(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`node_id`),
  UNIQUE KEY `cfg_nodes_idx01` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_options`
--

DROP TABLE IF EXISTS `cfg_options`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_options` (
  `group` varchar(255) NOT NULL DEFAULT 'default',
  `key` varchar(255) DEFAULT NULL,
  `value` varchar(255) DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_organizations`
--

DROP TABLE IF EXISTS `cfg_organizations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_organizations` (
  `organization_id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL,
  `shortname` varchar(100) NOT NULL,
  `active` tinyint(4) DEFAULT '1',
  PRIMARY KEY (`organization_id`),
  UNIQUE KEY `cfg_organizations_idx01` (`name`),
  UNIQUE KEY `cfg_organizations_idx02` (`shortname`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_organizations_modules_relations`
--

DROP TABLE IF EXISTS `cfg_organizations_modules_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_organizations_modules_relations` (
  `organization_id` int(11) NOT NULL,
  `module_id` int(10) unsigned NOT NULL,
  `is_activated` tinyint(4) NOT NULL DEFAULT '0',
  PRIMARY KEY (`organization_id`,`module_id`),
  KEY `FI__organizations_modules_relations_fk02` (`module_id`),
  CONSTRAINT `cfg_organizations_modules_relations_fk01` FOREIGN KEY (`organization_id`) REFERENCES `cfg_organizations` (`organization_id`) ON DELETE CASCADE,
  CONSTRAINT `cfg_organizations_modules_relations_fk02` FOREIGN KEY (`module_id`) REFERENCES `cfg_modules` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_organizations_users_relations`
--

DROP TABLE IF EXISTS `cfg_organizations_users_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_organizations_users_relations` (
  `organization_id` int(11) NOT NULL,
  `user_id` int(10) unsigned NOT NULL,
  `is_default` tinyint(4) DEFAULT '0',
  `is_admin` tinyint(4) DEFAULT '0',
  PRIMARY KEY (`organization_id`,`user_id`),
  KEY `FI__organizations_users_relations_fk02` (`user_id`),
  CONSTRAINT `cfg_organizations_users_relations_fk01` FOREIGN KEY (`organization_id`) REFERENCES `cfg_organizations` (`organization_id`) ON DELETE CASCADE,
  CONSTRAINT `cfg_organizations_users_relations_fk02` FOREIGN KEY (`user_id`) REFERENCES `cfg_users` (`user_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_organizations_widget_models_relations`
--

DROP TABLE IF EXISTS `cfg_organizations_widget_models_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_organizations_widget_models_relations` (
  `organization_id` int(11) NOT NULL,
  `widget_model_id` int(11) NOT NULL,
  PRIMARY KEY (`organization_id`,`widget_model_id`),
  KEY `FI__organizations_widget_models_relations_fk02` (`widget_model_id`),
  CONSTRAINT `cfg_organizations_widget_models_relations_fk01` FOREIGN KEY (`organization_id`) REFERENCES `cfg_organizations` (`organization_id`) ON DELETE CASCADE,
  CONSTRAINT `cfg_organizations_widget_models_relations_fk02` FOREIGN KEY (`widget_model_id`) REFERENCES `cfg_widgets_models` (`widget_model_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_pollers`
--

DROP TABLE IF EXISTS `cfg_pollers`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_pollers` (
  `poller_id` int(11) NOT NULL AUTO_INCREMENT,
  `node_id` int(11) NOT NULL,
  `organization_id` int(11) NOT NULL,
  `name` varchar(255) NOT NULL,
  `port` int(11) NOT NULL,
  `one_per_retention` int(1) NOT NULL DEFAULT '1',
  `tmpl_name` varchar(50) NOT NULL,
  `enable` int(1) NOT NULL DEFAULT '1',
  PRIMARY KEY (`poller_id`),
  UNIQUE KEY `cfg_pollers_idx01` (`name`),
  KEY `cfg_pollers_FI_1` (`organization_id`),
  KEY `cfg_pollers_FI_2` (`node_id`),
  CONSTRAINT `cfg_pollers_FK_1` FOREIGN KEY (`organization_id`) REFERENCES `cfg_organizations` (`organization_id`) ON DELETE CASCADE,
  CONSTRAINT `cfg_pollers_FK_2` FOREIGN KEY (`node_id`) REFERENCES `cfg_nodes` (`node_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_pollers_commands_relations`
--

DROP TABLE IF EXISTS `cfg_pollers_commands_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_pollers_commands_relations` (
  `poller_id` int(11) NOT NULL,
  `command_id` int(11) NOT NULL,
  `command_order` tinyint(3) DEFAULT NULL,
  KEY `poller_id` (`poller_id`),
  KEY `command_id` (`command_id`),
  CONSTRAINT `cfg_pollers_commands_relations_ibfk_1` FOREIGN KEY (`poller_id`) REFERENCES `cfg_pollers` (`poller_id`) ON DELETE CASCADE,
  CONSTRAINT `cfg_pollers_commands_relations_ibfk_2` FOREIGN KEY (`command_id`) REFERENCES `cfg_commands` (`command_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_resources`
--

DROP TABLE IF EXISTS `cfg_resources`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_resources` (
  `resource_id` int(11) NOT NULL AUTO_INCREMENT,
  `resource_name` varchar(255) DEFAULT NULL,
  `resource_line` varchar(255) DEFAULT NULL,
  `resource_comment` varchar(255) DEFAULT NULL,
  `resource_activate` enum('0','1') DEFAULT NULL,
  `organization_id` int(11) NOT NULL,
  PRIMARY KEY (`resource_id`),
  KEY `FI_ources_ibfk_1` (`organization_id`),
  CONSTRAINT `resources_ibfk_1` FOREIGN KEY (`organization_id`) REFERENCES `cfg_organizations` (`organization_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_resources_instances_relations`
--

DROP TABLE IF EXISTS `cfg_resources_instances_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_resources_instances_relations` (
  `resource_id` int(11) NOT NULL,
  `instance_id` int(11) NOT NULL,
  KEY `resource_id` (`resource_id`),
  KEY `instance_id` (`instance_id`),
  CONSTRAINT `cfg_resources_instances_relations_ibfk_1` FOREIGN KEY (`resource_id`) REFERENCES `cfg_resources` (`resource_id`) ON DELETE CASCADE,
  CONSTRAINT `cfg_resources_instances_relations_ibfk_2` FOREIGN KEY (`instance_id`) REFERENCES `cfg_pollers` (`poller_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_searches`
--

DROP TABLE IF EXISTS `cfg_searches`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_searches` (
  `search_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `user_id` int(10) unsigned NOT NULL,
  `route` varchar(255) NOT NULL,
  `label` varchar(255) NOT NULL,
  `searchText` text NOT NULL,
  PRIMARY KEY (`search_id`),
  UNIQUE KEY `searches_idx01` (`user_id`,`label`,`route`),
  CONSTRAINT `search_ibfk_1` FOREIGN KEY (`user_id`) REFERENCES `cfg_users` (`user_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_servicecategories`
--

DROP TABLE IF EXISTS `cfg_servicecategories`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_servicecategories` (
  `sc_id` int(11) NOT NULL AUTO_INCREMENT,
  `sc_name` varchar(255) DEFAULT NULL,
  `sc_description` varchar(255) DEFAULT NULL,
  `level` tinyint(5) DEFAULT NULL,
  `icon_id` int(11) DEFAULT NULL,
  `sc_comment` text,
  `sc_activate` enum('0','1') DEFAULT '1',
  `organization_id` int(11) NOT NULL,
  PRIMARY KEY (`sc_id`),
  UNIQUE KEY `sc_unique_01` (`sc_name`),
  KEY `FI_vicecategorie_ibfk_1` (`organization_id`),
  CONSTRAINT `servicecategorie_ibfk_1` FOREIGN KEY (`organization_id`) REFERENCES `cfg_organizations` (`organization_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_servicecategories_relations`
--

DROP TABLE IF EXISTS `cfg_servicecategories_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_servicecategories_relations` (
  `scr_id` int(11) NOT NULL AUTO_INCREMENT,
  `service_service_id` int(11) DEFAULT NULL,
  `sc_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`scr_id`),
  KEY `service_service_id` (`service_service_id`),
  KEY `sc_id` (`sc_id`),
  CONSTRAINT `servicecategories_relation_ibfk_1` FOREIGN KEY (`service_service_id`) REFERENCES `cfg_services` (`service_id`) ON DELETE CASCADE,
  CONSTRAINT `servicecategories_relation_ibfk_2` FOREIGN KEY (`sc_id`) REFERENCES `cfg_servicecategories` (`sc_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_servicegroups`
--

DROP TABLE IF EXISTS `cfg_servicegroups`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_servicegroups` (
  `sg_id` int(11) NOT NULL AUTO_INCREMENT,
  `sg_name` varchar(200) DEFAULT NULL,
  `sg_alias` varchar(200) DEFAULT NULL,
  `sg_comment` text,
  `sg_activate` enum('0','1') NOT NULL DEFAULT '1',
  `organization_id` int(11) NOT NULL,
  PRIMARY KEY (`sg_id`),
  UNIQUE KEY `sg_unique_01` (`sg_name`),
  KEY `name_index` (`sg_name`),
  KEY `alias_index` (`sg_alias`),
  KEY `FI_vicegroup_ibfk_1` (`organization_id`),
  CONSTRAINT `servicegroup_ibfk_1` FOREIGN KEY (`organization_id`) REFERENCES `cfg_organizations` (`organization_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_servicegroups_relations`
--

DROP TABLE IF EXISTS `cfg_servicegroups_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_servicegroups_relations` (
  `sgr_id` int(11) NOT NULL AUTO_INCREMENT,
  `host_host_id` int(11) DEFAULT NULL,
  `hostgroup_hg_id` int(11) DEFAULT NULL,
  `service_service_id` int(11) DEFAULT NULL,
  `servicegroup_sg_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`sgr_id`),
  KEY `service_index` (`service_service_id`),
  KEY `servicegroup_index` (`servicegroup_sg_id`),
  KEY `host_host_id` (`host_host_id`),
  KEY `hostgroup_hg_id` (`hostgroup_hg_id`),
  CONSTRAINT `cfg_servicegroup_relation_ibfk_8` FOREIGN KEY (`hostgroup_hg_id`) REFERENCES `cfg_hostgroups` (`hg_id`) ON DELETE CASCADE,
  CONSTRAINT `servicegroup_relation_ibfk_10` FOREIGN KEY (`servicegroup_sg_id`) REFERENCES `cfg_servicegroups` (`sg_id`) ON DELETE CASCADE,
  CONSTRAINT `servicegroup_relation_ibfk_7` FOREIGN KEY (`host_host_id`) REFERENCES `cfg_hosts` (`host_id`) ON DELETE CASCADE,
  CONSTRAINT `servicegroup_relation_ibfk_9` FOREIGN KEY (`service_service_id`) REFERENCES `cfg_services` (`service_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_services`
--

DROP TABLE IF EXISTS `cfg_services`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_services` (
  `service_id` int(11) NOT NULL AUTO_INCREMENT,
  `service_template_model_stm_id` int(11) DEFAULT NULL,
  `command_command_id` int(11) DEFAULT NULL,
  `timeperiod_tp_id` int(11) DEFAULT NULL,
  `command_command_id2` int(11) DEFAULT NULL,
  `timeperiod_tp_id2` int(11) DEFAULT NULL,
  `service_description` varchar(200) DEFAULT NULL,
  `service_alias` varchar(255) DEFAULT NULL,
  `display_name` varchar(255) DEFAULT NULL,
  `service_is_volatile` enum('0','1','2') DEFAULT '2',
  `service_max_check_attempts` int(11) DEFAULT NULL,
  `service_normal_check_interval` int(11) DEFAULT NULL,
  `service_retry_check_interval` int(11) DEFAULT NULL,
  `service_active_checks_enabled` enum('0','1','2') DEFAULT '2',
  `service_passive_checks_enabled` enum('0','1','2') DEFAULT '2',
  `initial_state` enum('o','w','u','c') DEFAULT NULL,
  `service_parallelize_check` enum('0','1','2') DEFAULT '2',
  `service_obsess_over_service` enum('0','1','2') DEFAULT '2',
  `service_check_freshness` enum('0','1','2') DEFAULT '2',
  `service_freshness_threshold` int(11) DEFAULT NULL,
  `service_event_handler_enabled` enum('0','1','2') DEFAULT '2',
  `service_low_flap_threshold` int(11) DEFAULT NULL,
  `service_high_flap_threshold` int(11) DEFAULT NULL,
  `service_flap_detection_enabled` enum('0','1','2') DEFAULT '2',
  `service_process_perf_data` enum('0','1','2') DEFAULT '2',
  `service_retain_status_information` enum('0','1','2') DEFAULT '2',
  `service_retain_nonstatus_information` enum('0','1','2') DEFAULT '2',
  `service_notification_interval` int(11) DEFAULT NULL,
  `service_notification_options` varchar(200) DEFAULT NULL,
  `service_notifications_enabled` enum('0','1','2') DEFAULT '2',
  `contact_additive_inheritance` tinyint(1) DEFAULT '0',
  `cg_additive_inheritance` tinyint(1) DEFAULT '0',
  `service_inherit_contacts_from_host` enum('0','1') DEFAULT '1',
  `service_first_notification_delay` int(11) DEFAULT NULL,
  `service_stalking_options` varchar(200) DEFAULT NULL,
  `service_comment` text,
  `command_command_id_arg` text,
  `command_command_id_arg2` text,
  `service_locked` tinyint(1) DEFAULT '0',
  `service_register` enum('0','1','2','3') NOT NULL DEFAULT '0',
  `service_activate` enum('0','1') NOT NULL DEFAULT '1',
  `organization_id` int(11) NOT NULL,
  `environment_id` int(11) DEFAULT NULL,
  `domain_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`service_id`),
  KEY `stm_index` (`service_template_model_stm_id`),
  KEY `cmd1_index` (`command_command_id`),
  KEY `cmd2_index` (`command_command_id2`),
  KEY `tp1_index` (`timeperiod_tp_id`),
  KEY `tp2_index` (`timeperiod_tp_id2`),
  KEY `description_index` (`service_description`),
  KEY `FI_vice_ibfk_5` (`organization_id`),
  KEY `FI_vice_ibfk_6` (`environment_id`),
  KEY `FI_vice_ibfk_7` (`domain_id`),
  CONSTRAINT `service_ibfk_1` FOREIGN KEY (`command_command_id`) REFERENCES `cfg_commands` (`command_id`) ON DELETE SET NULL,
  CONSTRAINT `service_ibfk_2` FOREIGN KEY (`command_command_id2`) REFERENCES `cfg_commands` (`command_id`) ON DELETE SET NULL,
  CONSTRAINT `service_ibfk_3` FOREIGN KEY (`timeperiod_tp_id`) REFERENCES `cfg_timeperiods` (`tp_id`) ON DELETE SET NULL,
  CONSTRAINT `service_ibfk_4` FOREIGN KEY (`timeperiod_tp_id2`) REFERENCES `cfg_timeperiods` (`tp_id`) ON DELETE SET NULL,
  CONSTRAINT `service_ibfk_5` FOREIGN KEY (`organization_id`) REFERENCES `cfg_organizations` (`organization_id`) ON DELETE CASCADE,
  CONSTRAINT `service_ibfk_6` FOREIGN KEY (`environment_id`) REFERENCES `cfg_environments` (`environment_id`) ON DELETE CASCADE,
  CONSTRAINT `service_ibfk_7` FOREIGN KEY (`domain_id`) REFERENCES `cfg_domains` (`domain_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_services_checkcmd_args_relations`
--

DROP TABLE IF EXISTS `cfg_services_checkcmd_args_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_services_checkcmd_args_relations` (
  `service_id` int(11) NOT NULL,
  `arg_number` tinyint(3) NOT NULL,
  `arg_value` varchar(255) DEFAULT NULL,
  KEY `fk_service_checkcmd_args_relations` (`service_id`),
  CONSTRAINT `fk_service_checkcmd_args_relations` FOREIGN KEY (`service_id`) REFERENCES `cfg_services` (`service_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_services_images_relations`
--

DROP TABLE IF EXISTS `cfg_services_images_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_services_images_relations` (
  `service_id` int(11) NOT NULL,
  `binary_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`service_id`,`binary_id`),
  KEY `FI_vice_image_relation_ibfk_2` (`binary_id`),
  CONSTRAINT `service_image_relation_ibfk_1` FOREIGN KEY (`service_id`) REFERENCES `cfg_services` (`service_id`) ON DELETE CASCADE,
  CONSTRAINT `service_image_relation_ibfk_2` FOREIGN KEY (`binary_id`) REFERENCES `cfg_binaries` (`binary_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_sessions`
--

DROP TABLE IF EXISTS `cfg_sessions`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_sessions` (
  `session_id` varchar(255) NOT NULL,
  `user_id` int(11) NOT NULL,
  `session_start_time` int(11) NOT NULL,
  `last_reload` int(11) NOT NULL,
  `ip_address` varchar(45) NOT NULL,
  `route` varchar(255) NOT NULL,
  `update_acl` tinyint(1) DEFAULT '0',
  PRIMARY KEY (`session_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_tags`
--

DROP TABLE IF EXISTS `cfg_tags`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_tags` (
  `tag_id` int(11) NOT NULL AUTO_INCREMENT,
  `user_id` int(10) unsigned NOT NULL,
  `tagname` varchar(100) NOT NULL,
  PRIMARY KEY (`tag_id`),
  UNIQUE KEY `cfg_tags_idx_01` (`user_id`,`tagname`),
  CONSTRAINT `cfg_tags_fk_01` FOREIGN KEY (`user_id`) REFERENCES `cfg_users` (`user_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_tags_bas`
--

DROP TABLE IF EXISTS `cfg_tags_bas`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_tags_bas` (
  `tag_id` int(11) NOT NULL,
  `resource_id` int(11) NOT NULL,
  PRIMARY KEY (`tag_id`,`resource_id`),
  KEY `FI__tags_bas_fk_02` (`resource_id`),
  CONSTRAINT `cfg_tags_bas_fk_01` FOREIGN KEY (`tag_id`) REFERENCES `cfg_tags` (`tag_id`),
  CONSTRAINT `cfg_tags_bas_fk_02` FOREIGN KEY (`resource_id`) REFERENCES `cfg_bam` (`ba_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_tags_hostgroups`
--

DROP TABLE IF EXISTS `cfg_tags_hostgroups`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_tags_hostgroups` (
  `tag_id` int(11) NOT NULL,
  `resource_id` int(11) NOT NULL,
  PRIMARY KEY (`tag_id`,`resource_id`),
  KEY `FI__tags_hostgroups_fk_02` (`resource_id`),
  CONSTRAINT `cfg_tags_hostgroups_fk_01` FOREIGN KEY (`tag_id`) REFERENCES `cfg_tags` (`tag_id`),
  CONSTRAINT `cfg_tags_hostgroups_fk_02` FOREIGN KEY (`resource_id`) REFERENCES `cfg_hostgroups` (`hg_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_tags_hosts`
--

DROP TABLE IF EXISTS `cfg_tags_hosts`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_tags_hosts` (
  `tag_id` int(11) NOT NULL,
  `resource_id` int(11) NOT NULL,
  PRIMARY KEY (`tag_id`,`resource_id`),
  KEY `FI__tags_hosts_fk_02` (`resource_id`),
  CONSTRAINT `cfg_tags_hosts_fk_01` FOREIGN KEY (`tag_id`) REFERENCES `cfg_tags` (`tag_id`) ON DELETE CASCADE,
  CONSTRAINT `cfg_tags_hosts_fk_02` FOREIGN KEY (`resource_id`) REFERENCES `cfg_hosts` (`host_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_tags_servicegroups`
--

DROP TABLE IF EXISTS `cfg_tags_servicegroups`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_tags_servicegroups` (
  `tag_id` int(11) NOT NULL,
  `resource_id` int(11) NOT NULL,
  PRIMARY KEY (`tag_id`,`resource_id`),
  KEY `FI__tags_servicegroups_fk_02` (`resource_id`),
  CONSTRAINT `cfg_tags_servicegroups_fk_01` FOREIGN KEY (`tag_id`) REFERENCES `cfg_tags` (`tag_id`),
  CONSTRAINT `cfg_tags_servicegroups_fk_02` FOREIGN KEY (`resource_id`) REFERENCES `cfg_servicegroups` (`sg_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_tags_services`
--

DROP TABLE IF EXISTS `cfg_tags_services`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_tags_services` (
  `tag_id` int(11) NOT NULL,
  `resource_id` int(11) NOT NULL,
  PRIMARY KEY (`tag_id`,`resource_id`),
  KEY `FI__tags_services_fk_02` (`resource_id`),
  CONSTRAINT `cfg_tags_services_fk_01` FOREIGN KEY (`tag_id`) REFERENCES `cfg_tags` (`tag_id`),
  CONSTRAINT `cfg_tags_services_fk_02` FOREIGN KEY (`resource_id`) REFERENCES `cfg_services` (`service_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_timeperiods`
--

DROP TABLE IF EXISTS `cfg_timeperiods`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_timeperiods` (
  `tp_id` int(11) NOT NULL AUTO_INCREMENT,
  `tp_name` varchar(200) DEFAULT NULL,
  `tp_alias` varchar(200) DEFAULT NULL,
  `tp_sunday` varchar(200) DEFAULT NULL,
  `tp_monday` varchar(200) DEFAULT NULL,
  `tp_tuesday` varchar(200) DEFAULT NULL,
  `tp_wednesday` varchar(200) DEFAULT NULL,
  `tp_thursday` varchar(200) DEFAULT NULL,
  `tp_friday` varchar(200) DEFAULT NULL,
  `tp_saturday` varchar(200) DEFAULT NULL,
  `organization_id` int(11) NOT NULL,
  PRIMARY KEY (`tp_id`),
  UNIQUE KEY `tp_unique_01` (`tp_name`),
  KEY `FI_eperiod_ibfk_1` (`organization_id`),
  CONSTRAINT `timeperiod_ibfk_1` FOREIGN KEY (`organization_id`) REFERENCES `cfg_organizations` (`organization_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_timeperiods_exceptions`
--

DROP TABLE IF EXISTS `cfg_timeperiods_exceptions`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_timeperiods_exceptions` (
  `exception_id` int(11) NOT NULL AUTO_INCREMENT,
  `timeperiod_id` int(11) NOT NULL,
  `days` varchar(255) NOT NULL,
  `timerange` varchar(255) NOT NULL,
  PRIMARY KEY (`exception_id`),
  KEY `timeperiod_exceptions_relation_ibfk_1` (`timeperiod_id`),
  CONSTRAINT `timeperiod_exceptions_relation_ibfk_1` FOREIGN KEY (`timeperiod_id`) REFERENCES `cfg_timeperiods` (`tp_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_timeperiods_exclude_relations`
--

DROP TABLE IF EXISTS `cfg_timeperiods_exclude_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_timeperiods_exclude_relations` (
  `exclude_id` int(11) NOT NULL AUTO_INCREMENT,
  `timeperiod_id` int(11) NOT NULL,
  `timeperiod_exclude_id` int(11) NOT NULL,
  PRIMARY KEY (`exclude_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_timeperiods_include_relations`
--

DROP TABLE IF EXISTS `cfg_timeperiods_include_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_timeperiods_include_relations` (
  `include_id` int(11) NOT NULL AUTO_INCREMENT,
  `timeperiod_id` int(11) NOT NULL,
  `timeperiod_include_id` int(11) NOT NULL,
  PRIMARY KEY (`include_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_timezones`
--

DROP TABLE IF EXISTS `cfg_timezones`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_timezones` (
  `timezone_id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(200) NOT NULL,
  `offset` varchar(200) NOT NULL,
  `dst_offset` varchar(200) NOT NULL,
  `description` varchar(200) DEFAULT NULL,
  PRIMARY KEY (`timezone_id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_traps`
--

DROP TABLE IF EXISTS `cfg_traps`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_traps` (
  `traps_id` int(11) NOT NULL AUTO_INCREMENT,
  `traps_name` varchar(255) DEFAULT NULL,
  `traps_oid` varchar(255) DEFAULT NULL,
  `traps_args` text,
  `traps_status` enum('-1','0','1','2','3') DEFAULT NULL,
  `severity_id` int(11) DEFAULT NULL,
  `manufacturer_id` int(11) DEFAULT NULL,
  `traps_reschedule_svc_enable` enum('0','1') DEFAULT '0',
  `traps_execution_command` varchar(255) DEFAULT NULL,
  `traps_execution_command_enable` enum('0','1') DEFAULT '0',
  `traps_submit_result_enable` enum('0','1') DEFAULT '0',
  `traps_advanced_treatment` enum('0','1') DEFAULT '0',
  `traps_advanced_treatment_default` enum('0','1') DEFAULT '0',
  `traps_timeout` int(11) DEFAULT NULL,
  `traps_exec_interval` int(11) DEFAULT NULL,
  `traps_exec_interval_type` enum('0','1','2') DEFAULT '0',
  `traps_log` enum('0','1') DEFAULT '0',
  `traps_routing_mode` enum('0','1') DEFAULT '0',
  `traps_routing_value` varchar(255) DEFAULT NULL,
  `traps_exec_method` enum('0','1') DEFAULT '0',
  `traps_comments` text,
  `organization_id` int(11) NOT NULL,
  UNIQUE KEY `traps_name` (`traps_name`),
  KEY `traps_id` (`traps_id`),
  KEY `traps_ibfk_1` (`manufacturer_id`),
  KEY `traps_ibfk_2` (`severity_id`),
  KEY `FI__traps_ibfk_3` (`organization_id`),
  CONSTRAINT `cfg_traps_ibfk_3` FOREIGN KEY (`organization_id`) REFERENCES `cfg_organizations` (`organization_id`) ON DELETE CASCADE,
  CONSTRAINT `traps_ibfk_1` FOREIGN KEY (`manufacturer_id`) REFERENCES `cfg_traps_vendors` (`id`) ON DELETE CASCADE,
  CONSTRAINT `traps_ibfk_2` FOREIGN KEY (`severity_id`) REFERENCES `cfg_servicecategories` (`sc_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_traps_matching_properties`
--

DROP TABLE IF EXISTS `cfg_traps_matching_properties`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_traps_matching_properties` (
  `tmo_id` int(11) NOT NULL AUTO_INCREMENT,
  `trap_id` int(11) DEFAULT NULL,
  `tmo_order` int(11) DEFAULT NULL,
  `tmo_regexp` varchar(255) DEFAULT NULL,
  `tmo_string` varchar(255) DEFAULT NULL,
  `tmo_status` int(11) DEFAULT NULL,
  `severity_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`tmo_id`),
  KEY `trap_id` (`trap_id`),
  KEY `traps_matching_properties_ibfk_2` (`severity_id`),
  CONSTRAINT `cfg_traps_matching_properties_ibfk_1` FOREIGN KEY (`trap_id`) REFERENCES `cfg_traps` (`traps_id`) ON DELETE CASCADE,
  CONSTRAINT `cfg_traps_matching_properties_ibfk_2` FOREIGN KEY (`severity_id`) REFERENCES `cfg_servicecategories` (`sc_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_traps_preexec`
--

DROP TABLE IF EXISTS `cfg_traps_preexec`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_traps_preexec` (
  `trap_id` int(11) DEFAULT NULL,
  `tpe_order` int(11) DEFAULT NULL,
  `tpe_string` varchar(512) DEFAULT NULL,
  KEY `trap_id` (`trap_id`),
  CONSTRAINT `traps_preexec_ibfk_1` FOREIGN KEY (`trap_id`) REFERENCES `cfg_traps` (`traps_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_traps_services_relations`
--

DROP TABLE IF EXISTS `cfg_traps_services_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_traps_services_relations` (
  `tsr_id` int(11) NOT NULL AUTO_INCREMENT,
  `traps_id` int(11) DEFAULT NULL,
  `service_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`tsr_id`),
  KEY `service_index` (`service_id`),
  KEY `traps_index` (`traps_id`),
  CONSTRAINT `traps_service_relation_ibfk_2` FOREIGN KEY (`service_id`) REFERENCES `cfg_services` (`service_id`) ON DELETE CASCADE,
  CONSTRAINT `traps_service_relation_ibfk_3` FOREIGN KEY (`traps_id`) REFERENCES `cfg_traps` (`traps_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_traps_vendors`
--

DROP TABLE IF EXISTS `cfg_traps_vendors`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_traps_vendors` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(254) NOT NULL,
  `alias` varchar(254) DEFAULT NULL,
  `description` text,
  `organization_id` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `unique-name` (`name`),
  KEY `FI_ps_vendors_ibfk_1` (`organization_id`),
  CONSTRAINT `traps_vendors_ibfk_1` FOREIGN KEY (`organization_id`) REFERENCES `cfg_organizations` (`organization_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_users`
--

DROP TABLE IF EXISTS `cfg_users`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_users` (
  `user_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `login` varchar(200) NOT NULL,
  `password` varchar(255) NOT NULL,
  `is_admin` tinyint(4) NOT NULL DEFAULT '0',
  `is_locked` tinyint(4) NOT NULL DEFAULT '0',
  `is_activated` tinyint(4) NOT NULL DEFAULT '0',
  `is_password_old` tinyint(4) NOT NULL DEFAULT '0',
  `language_id` int(11) DEFAULT NULL,
  `timezone_id` int(11) DEFAULT NULL,
  `contact_id` int(11) DEFAULT NULL,
  `createdat` datetime NOT NULL,
  `updatedat` datetime NOT NULL,
  `auth_type` varchar(200) NOT NULL,
  `firstname` varchar(200) DEFAULT NULL,
  `lastname` varchar(200) DEFAULT NULL,
  `autologin_key` varchar(200) DEFAULT NULL,
  PRIMARY KEY (`user_id`),
  UNIQUE KEY `user_login` (`login`),
  KEY `user_language_ibfk_1` (`language_id`),
  KEY `user_timezone_ibfk_1` (`timezone_id`),
  KEY `user_contact_ibfk_1` (`contact_id`),
  CONSTRAINT `user_contact_ibfk_1` FOREIGN KEY (`contact_id`) REFERENCES `cfg_contacts` (`contact_id`) ON DELETE SET NULL,
  CONSTRAINT `user_language_ibfk_1` FOREIGN KEY (`language_id`) REFERENCES `cfg_languages` (`language_id`) ON DELETE SET NULL,
  CONSTRAINT `user_timezone_ibfk_1` FOREIGN KEY (`timezone_id`) REFERENCES `cfg_timezones` (`timezone_id`) ON DELETE SET NULL
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_view_images`
--

DROP TABLE IF EXISTS `cfg_view_images`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_view_images` (
  `img_id` int(11) NOT NULL AUTO_INCREMENT,
  `img_name` varchar(255) DEFAULT NULL,
  `img_path` varchar(255) DEFAULT NULL,
  `img_comment` text,
  PRIMARY KEY (`img_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_view_images_dir`
--

DROP TABLE IF EXISTS `cfg_view_images_dir`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_view_images_dir` (
  `dir_id` int(11) NOT NULL AUTO_INCREMENT,
  `dir_name` varchar(255) DEFAULT NULL,
  `dir_alias` varchar(255) DEFAULT NULL,
  `dir_comment` text,
  PRIMARY KEY (`dir_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_view_images_dir_relations`
--

DROP TABLE IF EXISTS `cfg_view_images_dir_relations`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_view_images_dir_relations` (
  `vidr_id` int(11) NOT NULL AUTO_INCREMENT,
  `dir_dir_parent_id` int(11) DEFAULT NULL,
  `img_img_id` int(11) DEFAULT NULL,
  PRIMARY KEY (`vidr_id`),
  KEY `directory_parent_index` (`dir_dir_parent_id`),
  KEY `image_index` (`img_img_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_virtual_metrics`
--

DROP TABLE IF EXISTS `cfg_virtual_metrics`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_virtual_metrics` (
  `vmetric_id` int(11) NOT NULL AUTO_INCREMENT,
  `index_id` int(11) DEFAULT NULL,
  `vmetric_name` varchar(255) DEFAULT NULL,
  `def_type` enum('0','1') DEFAULT '0',
  `rpn_function` varchar(255) DEFAULT NULL,
  `warn` int(11) DEFAULT NULL,
  `crit` int(11) DEFAULT NULL,
  `unit_name` varchar(32) DEFAULT NULL,
  `hidden` enum('0','1') DEFAULT '0',
  `comment` text,
  `vmetric_activate` enum('0','1') DEFAULT NULL,
  `ck_state` enum('0','1','2') DEFAULT NULL,
  `organization_id` int(11) NOT NULL,
  PRIMARY KEY (`vmetric_id`),
  KEY `FI_tual_metrics_ibfk_1` (`organization_id`),
  CONSTRAINT `virtual_metrics_ibfk_1` FOREIGN KEY (`organization_id`) REFERENCES `cfg_organizations` (`organization_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_widgets`
--

DROP TABLE IF EXISTS `cfg_widgets`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_widgets` (
  `widget_id` int(11) NOT NULL AUTO_INCREMENT,
  `widget_model_id` int(11) NOT NULL,
  `title` varchar(255) NOT NULL,
  `custom_view_id` int(11) NOT NULL,
  `organization_id` int(11) NOT NULL,
  PRIMARY KEY (`widget_id`),
  KEY `fk_wdg_model_id` (`widget_model_id`),
  KEY `fk_widget_custom_view_id` (`custom_view_id`),
  KEY `FI_get_ibfk_1` (`organization_id`),
  CONSTRAINT `fk_wdg_model_id` FOREIGN KEY (`widget_model_id`) REFERENCES `cfg_widgets_models` (`widget_model_id`) ON DELETE CASCADE,
  CONSTRAINT `fk_widget_custom_view_id` FOREIGN KEY (`custom_view_id`) REFERENCES `cfg_custom_views` (`custom_view_id`) ON DELETE CASCADE,
  CONSTRAINT `widget_ibfk_1` FOREIGN KEY (`organization_id`) REFERENCES `cfg_organizations` (`organization_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_widgets_models`
--

DROP TABLE IF EXISTS `cfg_widgets_models`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_widgets_models` (
  `widget_model_id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL,
  `shortname` varchar(255) NOT NULL,
  `description` varchar(255) NOT NULL,
  `version` varchar(255) NOT NULL,
  `directory` varchar(255) NOT NULL,
  `author` varchar(255) NOT NULL,
  `email` varchar(255) DEFAULT NULL,
  `website` varchar(255) DEFAULT NULL,
  `keywords` varchar(255) DEFAULT NULL,
  `screenshot` varchar(255) DEFAULT NULL,
  `thumbnail` varchar(255) DEFAULT NULL,
  `isactivated` tinyint(1) DEFAULT '0',
  `isinstalled` tinyint(1) DEFAULT '0',
  `module_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`widget_model_id`,`module_id`),
  KEY `FI_widget_module_1` (`module_id`),
  CONSTRAINT `fk_widget_module_1` FOREIGN KEY (`module_id`) REFERENCES `cfg_modules` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_widgets_parameters`
--

DROP TABLE IF EXISTS `cfg_widgets_parameters`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_widgets_parameters` (
  `parameter_id` int(11) NOT NULL AUTO_INCREMENT,
  `parameter_name` varchar(255) NOT NULL,
  `parameter_code_name` varchar(255) NOT NULL,
  `default_value` varchar(255) DEFAULT NULL,
  `parameter_order` tinyint(6) NOT NULL,
  `header_title` varchar(255) DEFAULT NULL,
  `require_permission` varchar(255) NOT NULL,
  `widget_model_id` int(11) NOT NULL,
  `field_type_id` int(11) NOT NULL,
  `is_filter` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`parameter_id`),
  KEY `fk_widget_param_widget_id` (`widget_model_id`),
  KEY `fk_widget_field_type_id` (`field_type_id`),
  CONSTRAINT `fk_widget_field_type_id` FOREIGN KEY (`field_type_id`) REFERENCES `cfg_widgets_parameters_fields_types` (`field_type_id`) ON DELETE CASCADE,
  CONSTRAINT `fk_widget_param_widget_id` FOREIGN KEY (`widget_model_id`) REFERENCES `cfg_widgets_models` (`widget_model_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_widgets_parameters_fields_types`
--

DROP TABLE IF EXISTS `cfg_widgets_parameters_fields_types`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_widgets_parameters_fields_types` (
  `field_type_id` int(11) NOT NULL AUTO_INCREMENT,
  `ft_typename` varchar(50) NOT NULL,
  `is_connector` tinyint(6) NOT NULL DEFAULT '0',
  PRIMARY KEY (`field_type_id`)
) ENGINE=InnoDB AUTO_INCREMENT=16 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_widgets_parameters_multiple_options`
--

DROP TABLE IF EXISTS `cfg_widgets_parameters_multiple_options`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_widgets_parameters_multiple_options` (
  `parameter_id` int(11) NOT NULL,
  `option_name` varchar(255) NOT NULL,
  `option_value` varchar(255) NOT NULL,
  KEY `fk_option_parameter_id` (`parameter_id`),
  CONSTRAINT `fk_option_parameter_id` FOREIGN KEY (`parameter_id`) REFERENCES `cfg_widgets_parameters` (`parameter_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_widgets_parameters_range`
--

DROP TABLE IF EXISTS `cfg_widgets_parameters_range`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_widgets_parameters_range` (
  `parameter_id` int(11) NOT NULL,
  `min_range` int(11) NOT NULL,
  `max_range` int(11) NOT NULL,
  `step` int(11) NOT NULL,
  KEY `fk_option_range_id` (`parameter_id`),
  CONSTRAINT `fk_option_range_id` FOREIGN KEY (`parameter_id`) REFERENCES `cfg_widgets_parameters` (`parameter_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cfg_widgets_preferences`
--

DROP TABLE IF EXISTS `cfg_widgets_preferences`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cfg_widgets_preferences` (
  `widget_id` int(11) DEFAULT NULL,
  `parameter_id` int(11) NOT NULL,
  `preference_value` varchar(255) NOT NULL,
  `comparator` tinyint(4) DEFAULT NULL,
  KEY `fk_widget_parameter_id` (`parameter_id`),
  KEY `fk_widget_id` (`widget_id`),
  CONSTRAINT `fk_widget_id` FOREIGN KEY (`widget_id`) REFERENCES `cfg_widgets` (`widget_id`) ON DELETE CASCADE,
  CONSTRAINT `fk_widget_parameter_id` FOREIGN KEY (`parameter_id`) REFERENCES `cfg_widgets_parameters` (`parameter_id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_action`
--

DROP TABLE IF EXISTS `log_action`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_action` (
  `action_log_id` int(11) NOT NULL AUTO_INCREMENT,
  `action_log_date` int(11) NOT NULL,
  `object_type` varchar(255) NOT NULL,
  `object_id` int(11) NOT NULL,
  `object_name` varchar(255) NOT NULL,
  `action_type` varchar(255) NOT NULL,
  `log_contact_id` int(11) NOT NULL,
  PRIMARY KEY (`action_log_id`),
  KEY `log_contact_id` (`log_contact_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_action_modification`
--

DROP TABLE IF EXISTS `log_action_modification`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_action_modification` (
  `modification_id` int(11) NOT NULL AUTO_INCREMENT,
  `field_name` varchar(255) NOT NULL,
  `field_value` varchar(255) NOT NULL,
  `action_log_id` int(11) NOT NULL,
  PRIMARY KEY (`modification_id`),
  KEY `action_log_id` (`action_log_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_archive_host`
--

DROP TABLE IF EXISTS `log_archive_host`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_archive_host` (
  `log_id` int(11) NOT NULL AUTO_INCREMENT,
  `host_id` int(11) DEFAULT NULL,
  `UPTimeScheduled` int(11) DEFAULT NULL,
  `UPnbEvent` int(11) DEFAULT NULL,
  `UPTimeAverageAck` int(11) NOT NULL,
  `UPTimeAverageRecovery` int(11) NOT NULL,
  `DOWNTimeScheduled` int(11) DEFAULT NULL,
  `DOWNnbEvent` int(11) DEFAULT NULL,
  `DOWNTimeAverageAck` int(11) NOT NULL,
  `DOWNTimeAverageRecovery` int(11) NOT NULL,
  `UNREACHABLETimeScheduled` int(11) DEFAULT NULL,
  `UNREACHABLEnbEvent` int(11) DEFAULT NULL,
  `UNREACHABLETimeAverageAck` int(11) NOT NULL,
  `UNREACHABLETimeAverageRecovery` int(11) NOT NULL,
  `UNDETERMINEDTimeScheduled` int(11) DEFAULT NULL,
  `MaintenanceTime` int(11) DEFAULT '0',
  `date_end` int(11) DEFAULT NULL,
  `date_start` int(11) DEFAULT NULL,
  PRIMARY KEY (`log_id`),
  UNIQUE KEY `log_id` (`log_id`),
  KEY `host_index` (`host_id`),
  KEY `date_end_index` (`date_end`),
  KEY `date_start_index` (`date_start`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_archive_last_status`
--

DROP TABLE IF EXISTS `log_archive_last_status`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_archive_last_status` (
  `host_id` int(11) DEFAULT NULL,
  `service_id` int(11) DEFAULT NULL,
  `host_name` varchar(255) DEFAULT NULL,
  `service_description` varchar(255) DEFAULT NULL,
  `status` varchar(255) DEFAULT NULL,
  `ctime` int(11) DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_archive_service`
--

DROP TABLE IF EXISTS `log_archive_service`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_archive_service` (
  `log_id` int(11) NOT NULL AUTO_INCREMENT,
  `host_id` int(11) NOT NULL DEFAULT '0',
  `service_id` int(11) NOT NULL DEFAULT '0',
  `OKTimeScheduled` int(11) NOT NULL DEFAULT '0',
  `OKnbEvent` int(11) NOT NULL DEFAULT '0',
  `OKTimeAverageAck` int(11) NOT NULL,
  `OKTimeAverageRecovery` int(11) NOT NULL,
  `WARNINGTimeScheduled` int(11) NOT NULL DEFAULT '0',
  `WARNINGnbEvent` int(11) NOT NULL DEFAULT '0',
  `WARNINGTimeAverageAck` int(11) NOT NULL,
  `WARNINGTimeAverageRecovery` int(11) NOT NULL,
  `UNKNOWNTimeScheduled` int(11) NOT NULL DEFAULT '0',
  `UNKNOWNnbEvent` int(11) NOT NULL DEFAULT '0',
  `UNKNOWNTimeAverageAck` int(11) NOT NULL,
  `UNKNOWNTimeAverageRecovery` int(11) NOT NULL,
  `CRITICALTimeScheduled` int(11) NOT NULL DEFAULT '0',
  `CRITICALnbEvent` int(11) NOT NULL DEFAULT '0',
  `CRITICALTimeAverageAck` int(11) NOT NULL,
  `CRITICALTimeAverageRecovery` int(11) NOT NULL,
  `UNDETERMINEDTimeScheduled` int(11) NOT NULL DEFAULT '0',
  `MaintenanceTime` int(11) DEFAULT '0',
  `date_start` int(11) DEFAULT NULL,
  `date_end` int(11) DEFAULT NULL,
  PRIMARY KEY (`log_id`),
  KEY `host_index` (`host_id`),
  KEY `service_index` (`service_id`),
  KEY `date_end_index` (`date_end`),
  KEY `date_start_index` (`date_start`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_data_bin`
--

DROP TABLE IF EXISTS `log_data_bin`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_data_bin` (
  `metric_id` int(11) DEFAULT NULL,
  `ctime` int(11) DEFAULT NULL,
  `value` float DEFAULT NULL,
  `status` tinyint DEFAULT NULL,
  KEY `index_metric` (`metric_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_logs`
--

DROP TABLE IF EXISTS `log_logs`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_logs` (
  `log_id` int(11) NOT NULL AUTO_INCREMENT,
  `ctime` int(11) DEFAULT NULL,
  `host_id` int(11) DEFAULT NULL,
  `host_name` varchar(255) DEFAULT NULL,
  `instance_name` varchar(255) NOT NULL,
  `issue_id` int(11) DEFAULT NULL,
  `msg_type` tinyint(4) DEFAULT NULL,
  `notification_cmd` varchar(255) DEFAULT NULL,
  `notification_contact` varchar(255) DEFAULT NULL,
  `output` text,
  `retry` int(11) DEFAULT NULL,
  `service_description` varchar(255) DEFAULT NULL,
  `service_id` int(11) DEFAULT NULL,
  `status` tinyint(4) DEFAULT NULL,
  `type` smallint(6) DEFAULT NULL,
  PRIMARY KEY (`log_id`),
  KEY `host_name` (`host_name`),
  KEY `service_description` (`service_description`),
  KEY `status` (`status`),
  KEY `instance_name` (`instance_name`),
  KEY `ctime` (`ctime`),
  KEY `rq1` (`host_id`,`service_id`,`msg_type`,`status`,`ctime`),
  KEY `rq2` (`host_id`,`msg_type`,`status`,`ctime`),
  KEY `host_id` (`host_id`,`service_id`,`msg_type`,`ctime`,`status`),
  KEY `host_id_2` (`host_id`,`msg_type`,`ctime`,`status`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_snmptt`
--

DROP TABLE IF EXISTS `log_snmptt`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_snmptt` (
  `trap_id` int(11) NOT NULL AUTO_INCREMENT,
  `trap_oid` text,
  `trap_ip` varchar(50) DEFAULT NULL,
  `trap_community` varchar(50) DEFAULT NULL,
  `trap_infos` text,
  PRIMARY KEY (`trap_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_traps`
--

DROP TABLE IF EXISTS `log_traps`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_traps` (
  `trap_id` int(11) NOT NULL AUTO_INCREMENT,
  `trap_time` int(11) DEFAULT NULL,
  `timeout` enum('0','1') DEFAULT NULL,
  `host_name` varchar(255) DEFAULT NULL,
  `ip_address` varchar(255) DEFAULT NULL,
  `agent_host_name` varchar(255) DEFAULT NULL,
  `agent_ip_address` varchar(255) DEFAULT NULL,
  `trap_oid` varchar(512) DEFAULT NULL,
  `trap_name` varchar(255) DEFAULT NULL,
  `vendor` varchar(255) DEFAULT NULL,
  `status` int(11) DEFAULT NULL,
  `severity_id` int(11) DEFAULT NULL,
  `severity_name` varchar(255) DEFAULT NULL,
  `output_message` varchar(2048) DEFAULT NULL,
  KEY `trap_id` (`trap_id`),
  KEY `trap_time` (`trap_time`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `log_traps_args`
--

DROP TABLE IF EXISTS `log_traps_args`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log_traps_args` (
  `fk_log_traps` int(11) NOT NULL,
  `arg_number` int(11) DEFAULT NULL,
  `arg_oid` varchar(255) DEFAULT NULL,
  `arg_value` varchar(255) DEFAULT NULL,
  `trap_time` int(11) DEFAULT NULL,
  KEY `fk_log_traps` (`fk_log_traps`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
