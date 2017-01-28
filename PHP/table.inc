<?php
	class Table {
		var $header;
		var $body;
		//CONSTRUCTOR
		function Table($in_id, $in_header){
			$this->tag="<TABLE id='$in_id' class='sortable' BORDER=1";
			$this->body="";
			$this->header="<TR>";
			for ($i=0; $i < count($in_header); $i++){
				$this->header.="<TH>$in_header[$i]</TH>";
			}
			$this->header.="</TR>\n";
		}
		function Add($in_row){
			$this->body.="<TR>";
			for ($i=0;$i < count($in_row); $i++){
				if (strpos($in_row[$i],'|') != FALSE) {
					list($key,$display) = explode("|",$in_row[$i],2);
					$this->body.="<TD sorttable_customkey='$key'>$display</TD>";
				}
				else {
					$this->body.="<TD>$in_row[$i]</TD>";
				}
			}
			$this->body.="</TR>\n";
		}
		function Hide() {
			$this->tag.= " style='display:none;'";

		}
		function Create(){
			$return_table=$this->tag.">".$this->header.$this->body."</TABLE>";
			return($return_table);
		}
	}
?>
