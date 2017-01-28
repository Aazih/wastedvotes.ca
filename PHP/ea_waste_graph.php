<?php
include ("jpgraph/src/jpgraph.php");
include ("jpgraph/src/jpgraph_pie.php");
include ("jpgraph/src/jpgraph_pie3d.php");

$vote    = explode(",",$_REQUEST['vote']);
$pname   = explode(",",$_REQUEST['pname']);
$pcolour = explode(",",$_REQUEST['pcolour']);

// Create the Pie Graph.
$graph = new PieGraph(600,340,"auto");
$graph->SetShadow();

// Set A title for the plot
$graph->title->Set("Wasted Votes By Party");
$graph->legend->Pos(0.1,0.2);

for ($i = 0; $i < sizeof($pcolour); $i++){
	$pcolour[$i] = "#".$pcolour[$i];
}

// Create pie plot
$p1 = new PiePlot3d($vote);
$p1->SetSliceColors($pcolour);
$p1->SetLegends($pname);
$p1->SetCenter(0.4);
$p1->SetAngle(30);
$p1->value->SetFormat('%01.1f%%');
$p1->value->SetFont(FF_FONT2);
$p1->value->HideZero();

$graph->Add($p1);
$graph->Stroke();
?>


