<?php
include ("jpgraph/src/jpgraph.php");
include ("jpgraph/src/jpgraph_pie.php");
include ("jpgraph/src/jpgraph_pie3d.php");

$total    = $_REQUEST['total'];
$waste    = $_REQUEST['waste'];

$counted  = $total-$waste;

// Create the Pie Graph.
$graph = new PieGraph(600,340,"auto");
$graph->SetShadow();

// Set A title for the plot
$graph->title->Set("Wasted Votes");
$graph->legend->Pos(0.1,0.2);

// Create pie plot
$plot = array($counted,$waste);
$p1 = new PiePlot3d($plot);
$p1->SetStartAngle(90);
$p1->SetSliceColors(array('purple'  ,  'maroon' ));
$p1->SetLegends(    array('Effective','Wasted'));
$p1->SetCenter(0.4);
$p1->SetAngle(30);
$p1->value->SetFormat('%01.1f%%');
$p1->value->SetFont(FF_FONT2);
$p1->value->HideZero();

$graph->Add($p1);
$graph->Stroke();
?>


