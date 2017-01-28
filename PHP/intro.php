<div class="content">
<H1 style="text-align:center; padding-bottom:5px;">Highlighting the unheard voices and distorted results created by Canada's electoral system</H1>
<?php
require_once ("login.inc");
require_once ("function.inc");
require_once ("sql_functions.inc");

drupal_add_js("JAVASCRIPT/sorttable.js");

$dbcnx = LoginProd();

$array_info = GetTotalInfo($dbcnx);

$TOTAL = $array_info[0];
$WASTE = $array_info[1];
$PERCENT = $WASTE/$TOTAL*100;

$WasteDisplay = number_format($WASTE);
$PercentDisplay = PercentConversion($PERCENT, 'ENGLISH');

echo "    <H4 style='text-align:center; padding-bottm:5px;'>Wasted Vote: A vote cast in an election that has no effect whatsoever on the result of that election</dd></dt></dl></H4>";

echo "<H3 style='text-align:center;'> $WasteDisplay Canadian votes wasted and counting </H3>\n";

echo "<p>That's the total number of Canadian votes that were wasted just in the elections tracked on the site. <em>$PercentDisplay of the total votes cast.</em></p>\n";

echo "<p>Votes are the bricks on which democratic systems are built. The means by which citizens participate in their governments.</p>
<p><em>But does every vote cast in Canada actually make a difference? Surprisingly no</em>, in the First Past the Post (FPTP) system that we inherited from Britain in 1792 most Canadian votes today have no affect whatsoever on election results.</p>
<p>Finding this out came as a great shock to me. I have voted in every election I have been able to and I was surprised when a friend of mine said to me that <em>I was wasting my time as my vote wouldn't make any difference anyway</em>. I was even more surprised to find out that he was right.</p>
<p>In FPTP the amount of support a party gets has no effect on how many seats it gets in parliament. It did not make sense to me how a party could win 60% of the seats and gain complete majority power even though it got only 40% of the votes until I realized that in FPTP all the votes a party gets are not important; all that matters are the votes that win a riding. All other votes are wasted which means that in many Canadian elections <em>more than half of the votes cast</em> are wasted.</p>

<p>The purpose of this site is to track all those wasted votes. The votes for Liberals in Alberta, Conservatives in large urban areas, the Green party in Calgary, the NDP in Quebec and many more besides. These are the voices that FPTP suppresses and need to be heard to create a better Canada, a Canada in which every voice matters.</p>
  </div> ";




echo "<H3 style='text-align:center;'>Most Wasted Votes</H3>\n";

echo "<p>Most of the time in our current system more than half of the votes cast in a district are wasted. The following table shows the worst of these from all the elections tracked on the site.</p>\n";
$array_waste = CreateTopWasteTable($dbcnx, "ALL","ALL","ALL");

$topFiveTable = $array_waste[0];
$totalOverFifty = $array_waste[2];

$totalSeats = GetTotalSeats($dbcnx);

$overFiftyWaste = $totalOverFifty/$totalSeats*100;
$overFiftyWaste = PercentConversion($overFiftyWaste, 'ENGLISH');

echo $topFiveTable;
echo "<em>Overall more than half of the votes cast in $totalOverFifty districts were wasted. $overFiftyWaste of all districts</em>";

?>
