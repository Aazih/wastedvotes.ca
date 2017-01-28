<?php
require_once("election_area.inc");
require_once("global.inc");
drupal_add_css("CSS/area.css", "theme");
drupal_add_css("CSS/dropdown.css", "theme");
$parliament_type = arg(2);
$parliament_number = arg(3);
$region_number = arg(4);
$display_level = arg(5);
$EA = EA_Factory(arg(2),arg(3),arg(4),arg(5));
drupal_set_title($EA->ReturnTitle());
$top_crumb = $EA->ReturnTopCrumb();
$breadcrumb[]= l(t('Home'), '<front>');
#$breadcrumb[]= l(t("$parliament_type"), "node/$ELECTION_AREA_NODE/$parliament_type/$parliament_number/0/TOP");
$breadcrumb[]= l(t("$top_crumb"), "node/$ELECTION_AREA_NODE/$parliament_type/$parliament_number/0/TOP");
if ($display_level != 'TOP'){
	if ($parliament_type == 'Federal'){
		$province_english_name = $EA->GetProvinceEnglishName();
		$province_id = $EA->GetProvinceId();
		$breadcrumb[] = l(t("$province_english_name"), "node/$ELECTION_AREA_NODE/$parliament_type/$parliament_number/$province_id/PROVINCE");
	}
	if ($display_level == 'REGION' || $display_level == 'DISTRICT'){
		$region_list = $EA->GetRegionList();
		for ($i=0; $i < count($region_list);$i++){
			$region_info = $region_list[$i];
			$breadcrumb[] = l(t("$region_info[0]"), "node/$ELECTION_AREA_NODE/$parliament_type/$parliament_number/$region_info[1]/REGION");
		}
	}
	if ($display_level == 'DISTRICT') {
		$district_english_name = $EA->GetDistrictEnglishName();
		$breadcrumb[] = l(t("$district_english_name"), "node/$ELECTION_AREA_NODE/$parliament_type/$parliament_number/$region_number/DISTRICT");
	}
}
drupal_set_breadcrumb($breadcrumb);

echo "<div id ='dropdownbox'>";
$EA->CreateDropDown();
echo "</div>\n\n\n";
echo "<div id ='generalbox'>";
$EA->GeneralInfo();
echo "</div>\n\n\n";
echo "<div id ='splashbox'>";
$EA->SplashInfo();
echo "</div>\n\n\n";
echo "<div id ='areastatsbox'>";
$EA->StatsInfo();
$EA->GetVoteSummary();
echo "</div>\n\n\n";
?>
