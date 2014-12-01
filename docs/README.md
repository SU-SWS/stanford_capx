#CAPx Documentation

##Install the Stanford CAPx module on your Drupal 7 site

The [Stanford CAPx module can be downloaded from GitHub >>] (https://github.com/SU-SWS/stanford_capx)

If you do not have an existing content type, it is recommend that you use the Stanford Person content type. You can [download the Stanford Person module from GitHub >>] (https://github.com/SU-SWS/stanford_person)

**Important Note:** This documentation will use the Stanford Person content type as an example.

##Enable Stanford CAPx and Stanford Person modules using Drush

1. Run the following Drush command: % drush en stanford_person stanford_capx -y

**Note:** Dependencies should be automagically handled.

##Enable Stanford CAPx and Stanford Person modules from the user interface

1. Click **Modules** and locate the Stanford Person and the Stanford CAPx modules
2. Check the box next to each of the modules to enable them
3. Click **Save configuration**

##Configure Stanford CAPx module

1. Navigate to Configuration > CAPx

###Connect

1. Click the **Connect** tab
2. In the **Authorization** field, enter your authentication information
3. In the **Advanced** field, accept the default values unless it becomes necessary to change them
4. Click **Save connection settings**
 
Learn more about the [Connect tab >>] (connect.md)

###Settings

1. Click the **Settings** tab
2. In the **Organization Codes** field, click **Sync Now**
3. In the **Synchronization settings** field, accept the default values unless it becomes necessary to change them
4. Click **Save settings**

Learn more about the [Settings tab >>] (settings.md)

###Mapping

1. Click the **Mapping** tab

####Create new mapping

1. Click **Create new mapping**
2. 

**Note:** The following table highlights some commonly used settings for the Stanford Person content type:

Label | CAPx API Path
--- | ---
Display Name |	$.displayName
First name |	$.names.legal.firstName
Middle name |	$.displayName
Last Name |	$.names.legal.lastName
Display Name |	$.names.legal.middleName
Profile Picture |	$.profilePhotos.bigger
Type |	$.titles.*.type
Profile / Bio |	$.bio.html
Title and Department | $.longTitle[0]
Degrees / Education |	$.education.*.label.text
File |	$.documents.cv
Email |	$.primaryContact.email
Phone |	$.primaryContact.phoneNumbers.*
Fax |	$.primaryContact.fax
Office Hours |	
Mailing Address | $.primaryContact.address
Mail Code |	
Personal Info Links Title | $.internetLinks.*.label.text
Personal Info Links URL | $.internetLinks.*.url
Faculty Status | 
Student Type | $.titles.*.type
Cohort |$.maintainers.*.title
Field of Study | $.education.*.fieldOfStudy
Dissertation Title | 
Graduation Year | $.education.*.yearIssued
Staff Type | $.titles.*.type

####Edit exisiting mapping

1.

####Delete mapping

1. 

Learn more about the [Mapping tab >>] (mapping.md)

###Importing

1. Click the **Importing** tab

####Create new importer

1. Click **Create new importer**
2. In the **Importer name** field, enter a unique name for the Importer
3. In the **Mapping** field, select the mapping from the dropdown that you would like to import this profile data

####Edit existing importer

1.

####Delete importer

1.

Learn more about the [Importing tab >>] (importing.md)

###Help

Use the Help tab for quick information and helpful tips on using and setting up the CAPx module.
