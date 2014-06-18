CAP_drupal
==========

1. Getting Started

* Some modules are required by the Stanford CAP API modules. Please make sure
  they have been downloaded to your Drupal installation. See the .info files for
  more information. They can download these automatically for you when you
  enable the modules using Drush. See https://github.com/drush-ops/drush
* Enable the Stanford CAP API, organizations and profiles module.
* Visit admin/config/cap/config/settings and enter your CAP credentials.
* Click on the Details tab (admin/config/cap/config).
* Verify your site can connect to the CAP API.
* Import the organizations from CAP (click the link on the Details page).
* Either select a group or individual profile to import.
* Now the process will take considerable time to create the many fields
  required to store a faculty profile and once that has finished, the actual
  profile(s) will be imported.

Warning: the process of creating fields takes a long time. This will be
converted to Batch (and Queue) API soon. In the meantime, please ensure that a
high PHP execution time is configured to allow this process time to complete. If
it does timeout, just refresh the page and the process to create fields will
continue to create fields where it stopped. This may take 10 minutes to
complete the first time that ALL fields are created. Synchronizing the
profiles doesn't take nearly as much time to complete.


2. Stanford CAP API module

This module provides some functions to connect to the Stanford CAP (Community
Academic Profile) API and some configuration pages to store authentication
information. The submodules are much more interesting.

For more information about CAP, see http://cap.stanford.edu


3. Stanford CAP API organizations module

In order to import academic profiles in a more meaningful way, you need to
import the list of organizations. Organizations are imported as a taxonomy. This
can be used standalone (without the profile related modules).


4. Stanford CAP API profiles module

This module provides the import and synchronization functionality for faculty
profiles. There are two components to the import; schema and data. The schema
import creates the fields in Drupal required to store a faculty profile from
CAP. The data import retrieves the actual profiles from CAP. Thankfully,
intimate knowledge of these parts are not required to get started.

From the configuration page you can select a group (working group) of faculty
profiles OR search for individual profiles you wish to synchronize.

Synchronization can be configured to run each time you view a profile OR during
nightly by using a cron job. This is selectable in the administraion interace.


5. Stanford CAP API profiles layout module (Experimental)

This part of the module is as yet incomplete. If you enable the module, you will
need to visit admin/config/cap/config/profile_test and click the "Synchronize
profiles layout" button. This uses the CAP layouts endpoint to create a display
mode for the faculty profile content type and provide a sensible layout based on
CAP's suggestion on what data should be displayed.

See admin/structure/types/manage/cap_stanford_profile/display/full
