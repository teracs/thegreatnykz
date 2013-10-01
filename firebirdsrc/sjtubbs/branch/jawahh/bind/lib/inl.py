act_zsk = 'key.active_zsk'
act_ksk = 'key.active_ksk'
dea_zsks = 'key.deactive_zsks'
dea_ksks = 'key.deactive_ksks'
pub_zsk = 'key.published_zsk'
pub_ksk = 'key.published_ksk'
del_zsk = 'key.deleted_zsk'
#del_ksk = 'key.deleted_ksk'

pub_chld_ksks = 'key.published_child_ksks'
wat_chld_ksks = 'key.waiting_child_ksks'

ZSK_STAT_NORMAL = 0
ZSK_STAT_TOPUB = 1
ZSK_STAT_TOACT = 2
ZSK_STAT_TODEL = 3
zsk_roll_stat = 'key.zsk_rollover_status'
KSK_STAT_NORMAL = 0
KSK_STAT_TOPUB = 1
KSK_STAT_TOACT = 2
ksk_roll_stat = 'key.ksk_rollover_status'
CHLD_KSK_STAT_NORMAL = 0
CHLD_KSK_STAT_TOPUB = 1

last_sign_time = 'sys.last_sign_time'
zf_last_mtime = 'sys.zone_file_last_modified_time'

last_ip_mtime = 'sys.last_ip_modified_time'

DEFAULTS = {
	act_zsk : None,
	act_ksk : None,
	dea_zsks : [],
	dea_ksks : [],
	pub_zsk : None,
	pub_ksk : None,
	del_zsk : None,
	#del_ksk : None,
	
	pub_chld_ksks : [],
	wat_chld_ksks : [],
	
	zsk_roll_stat : ZSK_STAT_NORMAL,
	ksk_roll_stat : KSK_STAT_NORMAL,

	last_sign_time : 0,
	zf_last_mtime : 0,
	last_ip_mtime : 0,
}

