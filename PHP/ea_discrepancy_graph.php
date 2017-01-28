<?php
include ("jpgraph/src/jpgraph.php");
include ("jpgraph/src/jpgraph_bar.php");
include ("global.inc");

$support = explode(",",$_REQUEST['support']);
$seat    = explode(",",$_REQUEST['seat']);
$pname   = explode(",",$_REQUEST['pname']);

$datay=$support;
$datay2=$seat;
$datax=$pname;

// Create the graph. 
$graph = new Graph($GRAPH_WIDTH,$GRAPH_HEIGHT);
$graph->title->Set('Seats won Vs. Popular support');


// Setup Y and Y2 scales with some "grace"	
$graph->SetScale("textlin");
$graph->yaxis->scale->SetAutoMax(100);

$graph->xaxis->SetTickLabels($datax);
$graph->xaxis->SetFont(FF_FONT1);

//$graph->ygrid->Show(true,true);
$graph->ygrid->SetColor('gray','lightgray@0.5');

// Setup graph colors
$graph->SetMarginColor('white');


// Create the "dummy" 0 bplot
//$bplotzero = new BarPlot($datazero);

// Create the "Y" axis group
$ybplot1 = new BarPlot($datay);
$ybplot1->SetFillColor('purple');
$ybplot1->SetLegend('Popular support');
$ybplot2 = new BarPlot($datay2);
$ybplot2->SetFillColor('maroon');
$ybplot2->SetLegend('Seats won');
$ybplot = new GroupBarPlot(array($ybplot1,$ybplot2));

// Add the grouped bar plots to the graph
$graph->Add($ybplot);

// .. and finally stroke the image back to browser
$graph->Stroke();
?>
