<?php
function create_drupal_election_form($election_array) {

	$display_array = array_reverse($election_array);
	$display_array['NOACTION'] = 'CHOOSE AN ELECTION';
	$display_array = array_reverse($display_array);

	$form['election_field'] = array(
	'#type' => 'select',
	'#options' => $display_array,
        '#attributes' => array('onchange'=>'this.form.submit()'),
	);
	$form['submit'] = array(
	'#type' => 'submit',
	'#prefix' => '<div class="submit-box"><noscript>',
	'#suffix' => '</noscript></div>',
	'#value' => t('Go to chosen election'),
	);
	return $form;
}
function create_drupal_election_form_submit($form_id, $form_values){
	include "global.inc";
	return "node/$ELECTION_AREA_NODE/".$form_values['election_field']."/0/TOP";
}
function create_drupal_province_form($election_array) {

	$display_array = array_reverse($election_array);
	$display_array['NOACTION'] = 'CHOOSE A PROVINCE';
	$display_array = array_reverse($display_array);

	$form['province_field'] = array(
	'#type' => 'select',
	'#options' => $display_array,
        '#attributes' => array('onchange'=>'this.form.submit()'),
	);
	$form['submit'] = array(
	'#type' => 'submit',
	'#prefix' => '<div class="submit-box"><noscript>',
	'#suffix' => '</noscript></div>',
	'#value' => t('Go to chosen province'),
	);
	return $form;
}
function create_drupal_province_form_submit($form_id, $form_values){
	include "global.inc";
	if ($form_values['province_field'] != 'NOACTION'){
		return "node/$ELECTION_AREA_NODE/".$form_values['province_field']."/PROVINCE";
	}
}
function create_drupal_district_form($election_array) {

	$display_array = array_reverse($election_array);
	$display_array['NOACTION'] = 'CHOOSE A DISTRICT';
	$display_array = array_reverse($display_array);

	$form['district_field'] = array(
	'#type' => 'select',
	'#options' => $display_array,
        '#attributes' => array('onchange'=>'this.form.submit()'),
	);
	$form['submit'] = array(
	'#type' => 'submit',
	'#prefix' => '<div class="submit-box"><noscript>',
	'#suffix' => '</noscript></div>',
	'#value' => t('Go to chosen district'),
	);
	return $form;
}
function create_drupal_district_form_submit($form_id, $form_values){
	include "global.inc";
	if ($form_values['district_field'] != 'NOACTION'){
		return "node/$ELECTION_AREA_NODE/".$form_values['district_field']."/DISTRICT";
	}
}

function create_drupal_region_form($election_array,$in_level) {
	$display_array = array_reverse($election_array);
	if ($in_level==1) { $label = 'CHOOSE A REGION'; }
	else { $label = 'CHOOSE A SUB-REGION';}
	$display_array['NOACTION'] = $label;
	$display_array = array_reverse($display_array);

	$form['region_field'] = array(
	'#type' => 'select',
	'#options' => $display_array,
        '#attributes' => array('onchange'=>'this.form.submit()'),
	);
	$form['submit'] = array(
	'#type' => 'submit',
	'#prefix' => '<div class="submit-box"><noscript>',
	'#suffix' => '</noscript></div>',
	'#value' => t('Go to chosen region'),
	);
	return $form;
}
function create_drupal_region_form_submit($form_id, $form_values){
	include "global.inc";
	if ($form_values['region_field'] != 'NOACTION'){
		return "node/$ELECTION_AREA_NODE/".$form_values['region_field']."/REGION";
	}
}
?>
