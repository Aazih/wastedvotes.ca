Greetings to anyone interested in reading this.

I created wastedvotes.ca back in 2007 to highlight the sheer number of votes that do not result in representation in a typical Canadian, or indeed any, FPTP election. 

In the time since it has become harder and harder for the Canadian political elite (read, the people who get false majorities and full power for four years) to deny the simple fact of how many Canadians are left unrepresented by the Canadian electoral system (they still do it of course). I do not know if my little site has played any part in this but I hope that it has at least in some minor way. 

That's enough of the intro. Read on for an overview of how the site works and the contents of this project.

Folders:

Data - This might the most useful to people interested in Canadian election data. The folder contains the raw data, in csv format, of all the elections tracked on the site. Earlier on I used to write code to parse the different data formats that each province and the federal government provide their election result data in. Later on I have settled on a fairly typical data format that I convert election data into and use a general script to parse and insert into the database. Both of these approaches have advantages and drawbacks. In the end massaging data into a standard format is far more feasible than mainpulating code however. The format that Elections Nova Scotia uses to dissmeninate elections results data is beyond horrific for example and I am not interested in writing and debugging code to acccommadate that travesty.

drupal - This is the folder that contains the drupal installation that the site is built on. It is quite outdated. There are a few folders that I placed in there that are not a part of drupal for ease of inclusion into the site. They bear special mention
    CSS - A few bits of CSS to ensure things are in their right places
    JAVASCRIPT - The site allows for switching between table displays and sortable tables. This is driven by bits of Javascript that I did not create but adapted from others. They are stored here. 

HTML - A few static pages that are of no interest to anyone I am sure.

IMAGES - Flags for Canada and all of its provinces and terrotories. Also images that are maps for all of Ontario's electoral districts in its 38th parliament as once upon I time I wanted to display them when viewing district level results. Abandoned that idea pretty quickly but the images are still there.

ONE_TIME - This is the folder where all the scripts I used to parse Elections Data (from the Data folder) to insert into the election database reside.
    PERL - The main parsing and database insertion logic is written in PERL because... well I did this in 2007 what do you expect? As can be seen (and mentioned in the overivew of the Data folder) I used to have logic specific to each elections data but later I moved to much more generic code that can be seen in the insert_*.pl files. These are not used in the day to day operation of the site so this can be ignored. If you do wish to make use of these then please note that you will need to change the include statements as they are not portable at all. 
    SHELL - Quick and dirty csh scripts that run the parse and insert perl logic in a semi automated fashion. Initially I got it in my head that I should have a master csh script that, when run, would parse all the data in the Data folder and recreate the database from scratch. Many years later this started taking far too long to run. It's much easier just to backup the database and just insert the new data in then drop the entire db and recreate from scrach. 
    SQL - A lot of the queries used to insert the basic scaffolding data for the actual elections results. 

PHP - This is the heart of the site (most of the rest is the heart of the database). This is the PHP code that parses the election results in the database and displays them by different geographic regions. It's somewhat object oriented and somewhat implements the Object Factory Design Pattern. This allows the area_drupal_content.php, which actually displays the page to be incredibly simple. It just calls the EA_Factory function with the URL parameters to get the appropriate object and just calls the inherited functions of the returned object. The object code in election_area.inc and election_object.inc contains all the SQL and HTML formatting code. THis is most computer sciency thing I've ever done. That UofT Software Engineering degree was good for something!

SCHEMA - This folder contains the sql that creates the relational database tables that store election data. Here's a quick overview

    PARLIMENT is the top level. Each election elects a parliament after all. Parliaments are of a parliament type which defines which province the parliament is for one or if it's federal.
    
    PARTY is where information for the various political parties is stored. The parliament type is important here as well. The Liberal party of Canada is distinct from the Liberal Party of Ontario for example.

    PARTY_LEADER stores who the party leaders are for a certain party for certain parliments

    PROVINCE is all of the provinces and territories in Canada.

    DISTRICTS is a listing of all the electoral districts for any given parliament. A weakness of the database is that there is no link between a district between different parliaments even if it remained exactly the same between elections.

    CONTEST is where the election results actually are. Each row in CONTEST tracks the number of votes a candidate receieved in a district that is for a parliament. One irritating issue in my database design is that I avoided redundancy just a bit too much by not putting the PARLIAMENT_ID of the district in this table instead keeping that information just in the DISTRICT table. This means any query to get the results for any parliament needs to join to DISTRICTS. Since the information in this database is updated only occassionally but queried far more commonly this is not a great design. I could fix this but WHO HAS THE TIME FOR THAT?

    REGION Is an optional geographic grouping of Districts within a province. It's incredibly useful but it's not easy to come up with regional groupings as they are not formally defined divisions but arbitrary ones. fs

