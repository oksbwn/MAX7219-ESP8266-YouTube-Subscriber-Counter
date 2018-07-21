<?php
function tidy_html($input_string) {
         
	$config = array('output-html'   => true,'indent' => true,'wrap'=> 800); 
            
    // Detect if Tidy is in configured    
    if( function_exists('tidy_get_release') ) {
        $tidy = new tidy;
        $tidy->parseString($input_string, $config, 'raw');
        $tidy->cleanRepair();
        $cleaned_html  = tidy_get_output($tidy); 
        } 
	else {
        # Tidy not configured for this Server
        $cleaned_html = $input_string;
	}
    return $cleaned_html;
}

function getFromPage($webAddress,$path){
	$source = file_get_contents($webAddress); //download the page 
	$clean_source = tidy_html($source);
	$doc = new DOMDocument;

	// suppress errors
	libxml_use_internal_errors(true);

	// load the html source from a string
	$doc->loadHTML($clean_source);
	$xpath = new DOMXPath($doc);
	$data="";
	$nodelist = $xpath->query($path);
	$node_counts = $nodelist->length; // count how many nodes returned
	if ($node_counts) { // it will be true if the count is more than 0
		foreach ($nodelist as $element) {
           $data= $data.$element->nodeValue . "\n";
		}
	}
	return $data;
	
}
echo "[YT:".(int)str_replace(",","",getFromPage("http://www.youtube.com/weargenius","//*[@id=\"c4-primary-header-contents\"]/div/div/div[2]/div/span/span[1]"))."  TW:".
		(int)getFromPage("http://www.twitter.com/wglabz","//*[@id=\"page-container\"]/div[1]/div/div[2]/div/div/div[2]/div/div/ul/li[2]/a/span[3]")."]";
?>