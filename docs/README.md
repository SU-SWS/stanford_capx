#CAPx Documentation

##Install Stanford CAPx and Stanford Person modules on your Drupal 7 site

1. Install CAPx module: https://github.com/SU-SWS/stanford_capx
2. Install Stanford Person module: https://github.com/SU-SWS/stanford_person

##Enable Stanford CAPx and Stanford Person modules using Drush

1. Run the following Drush command (dependencies should be automagically handled): % drush en stanford_person stanford_capx -y

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

###Settings

1. Click the **Settings** tab
2. In the **Organization Codes** field, click **Sync Now**
3. In the **Synchronization settings** field, accept the default values unless it becomes necessary to change them
4. Click **Save settings**

###Mapping

1. Click the **Mapping** tab

####Create new mapping

1. Click **Create new mapping**
2. 

The following table highlights some commonly used settings:

Label | CAPx API Path
--- | ---
Title |	$.displayName
Body | $.bio.html
Profile picture	| $.profilePhotos.bigger
Email | $.primaryContact.email
Last updated |$.lastModified
Cohort | $.maintainers.*.title
Job title short | $.shortTitle.label.text
Job title long | $.longTitle[0]
Degrees / education | $.education.*.label.text
Title and department | $.longTitle[0]
Fax | $.primaryContact.fax
CV - file | $.documents.cv
CV - link | $.documents.cv.url
Resume - file | $.documents.resume
Resume - link | $.documents.resume.url
First name legal | $.names.legal.firstName
First name preferred | $.names.preferred.firstName
Last name legal | $.names.legal.lastName
Last name preferred | $.names.preferred.lastName
Middle name legal | $.names.legal.middleName
Middle name preferred | $.names.preferred.middleName
Graduation year | $.education.*.yearIssued
Personal info links title | $.internetLinks.*.label.text
Personal info links url | $.internetLinks.*.url
Fields of interest | $.professionalInterests.text
Mailing address | $.primaryContact.address
Mailing address city | $.primaryContact.city
Mailing address state | $.primaryContact.state
Mailing address zip | $.primaryContact.zip
Office | $.primaryContact.officeName
Phone | $.primaryContact.phoneNumbers.*
Staff type | $.titles.*.type
Field of study | $.education.*.fieldOfStudy
Type | $.titles.*.type

####Edit exisiting mapping

1.

####Delete mapping

1. 

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

###Help

Use the Help tab for information on using and setting up the CAPx module.
