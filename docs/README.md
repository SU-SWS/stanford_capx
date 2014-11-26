#CAPx Documentation

##Install Stanford CAPx and Stanford Person modules on your Drupal 7 site

1. Install CAPx module (suggested directory: sites/all/modules/stanford) https://github.com/SU-SWS/stanford_capx
2. Install Stanford Person module (suggested directory: sites/all/modules/stanford) https://github.com/SU-SWS/stanford_person

##Enable Stanford CAPx and Stanford Person modules using Drush

1. Run the following Drush command (dependencies should be automagically handled): % drush en stanford_person stanford_capx -y

##Enable Stanford CAPx and Stanford Person modules from the user interface

1. Click Modules and locate the Stanford Person and the Stanford CAPx modules
2. Check the box next to each of the modules to enable them
3. Click Save configuration

##Configure Stanford CAPx module

1. Navigate to Configuration > CAPx

###Connect

1. Click the Connect tab
2. In the Authorization field, enter your authentication information
3. Click Save connection settings

###Settings

1. Click the Settings tab
2. In the Organization Codes field, click Sync Now
3. In the Synchronization settings field, accept the default values unless it becomes necessary to change them
4. Click Save configuration ###Mapping 1. Click the Mapping tab 2. Click Create new mapper

###Importing

###Help
