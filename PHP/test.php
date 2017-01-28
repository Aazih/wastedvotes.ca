<?php
echo "<BR>".getcwd() . "\n<BR>";
require_once("election_area.inc");
$EA = new EA_Top('Federal','LATEST');
echo "<HR>";
$EA->GeneralInfo();
$EA->SplashInfo();
echo "<HR>";
$EA->StatsInfo();
echo "<HR>";
$EA->GetVoteSummary();
echo "<HR>";
echo "<HR><HR>";

$EA_P = new EA_Province('Federal',39,6);
echo "<HR> Province<HR>";
$EA_P->GeneralInfo();
$EA_P->SplashInfo();
$EA_P->StatsInfo();
$EA_P->GetVoteSummary();

$EA_D = new EA_District('Federal',39,1);
echo "<HR> District<HR>";
$EA_D->GeneralInfo();
$EA_D->SplashInfo();
$EA_D->StatsInfo();
$EA_D->GetVoteSummary();
?>
