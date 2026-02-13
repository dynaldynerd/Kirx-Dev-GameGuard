# OnDQSRun Ambiguous Cases

No unresolved ambiguity remains for case 53.
`AutoMineMachineMng::request_db_query` now dispatches by `pdata[0]` and casts `pdata` to the mapped payload struct per sub-query case.

case 0 : _qry_case_amine_newowner
case 1 : _qry_case_amine_batterycharge
case 2 : _qry_case_amine_mineore
case 3 : _qry_case_amine_workstate
case 4 : _qry_case_amine_selore
case 5 : _qry_case_amine_battery_discharge
case 6 : _qry_case_amine_moveore
