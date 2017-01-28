DHTML menus uses javascript DHTML to reduce the number of page loads when using
nested menus; this is particularly useful with Drupal's administration system.

Ordinarily in Drupal, when you click on a menu with sub-items, you need to 
go to that page and do a page refresh to get the items that are beneath it.
With DHTML Menus, instead the sub-items are expanded as soon as you click 
on it. Additionally, it uses a cookie to remember what menus are open and
what menus are closed, so as you navigate around the site your menus remain
consistent.

To use this module:

1) Drop the entire directory into your modules directory or sites/all/modules 
   directory.
2) Enable the module at administer >> site building >> modules
3) Go to administer >> block. Each menu you have in your system will have
   a second version of it starting with DHTML:. For example, 'DHTML: Navigation'
   for the primary menu.
4) Enable one or more of these menus. If you like, disable the original, but
   you'll feel better if you wait and make sure that the new DHTML version of
   it works properly.

Enjoy!
