<?php
/**
 * @file
 * @author [author] <[email]>
 */

use CAPx\APILib\HTTPClient;
use CAPx\Drupal\Importer\EntityImporter;
use CAPx\Drupal\Importer\EntityImporterBatch;

use CAPx\Drupal\Mapper\EntityMapper;
use CAPx\Drupal\Mapper\FieldCollectionMapper;

use CAPx\Drupal\Util\CAPx;
use CAPx\Drupal\Util\CAPxMapper;
use CAPx\Drupal\Util\CAPxImporter;
use CAPx\Drupal\Util\CAPxConnection;

use CAPx\Drupal\Organizations\Orgs;

// Pages
// /////////////////////////////////////////////////////////////////////////////


/**
 * Stanford CAPx admin overview page menu callback.
 * @return string
 *   HTML representation of this page.
 */
function stanford_capx_admin_config() {
  // Connection Status Block.
  $connection_block = module_invoke('stanford_capx', 'block_view', 'connection_status');

  // Content Output.
  $output = render($connection_block['content']);
  $output .= "<h2>" . t('Importers') . "</h2>";
  $output .= views_embed_view('importers_overview_page');

  return $output;
}

/**
 * Stanford CAPX connect page menu callback.
 * @return string
 *   HTML representation of this page.
 */
function stanford_capx_admin_config_connect() {
  $output = "";

  // The settings form found in stanford_capx.forms.inc
  $form = drupal_get_form('stanford_capx_forms_connect_form');
  $output .= drupal_render($form);

  return $output;
}

/**
 * Settings page menu callback.
 * @return string
 *   HTML representation of this page.
 */
function stanford_capx_admin_config_settings() {

  $output = array();

  $has_orgs = \CAPx\Drupal\Organizations\Orgs::checkOrgs();
  if (!$has_orgs) {
    drupal_set_message(t('The organization codes are not available. You should sync with the API now.'), 'warning');
  }

  // Organizations
  // ---------------------------------------------------------------------------
  $form = array();
  $form['orggroup'] = array(
    '#type' => "fieldset",
    '#title' => t("Organizations"),
    '#description' => t("Organization specific actions. Before you are able to start using organizations you have to sync them with the CAP API server."),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['orggroup']['theactions']['#markup'] = "<p>" . t("Sync organization data with CAP API: ") . l(t('Sync Now'), 'admin/config/capx/organizations/sync', array('class' => array('btn button'), 'query' => array('destination' => current_path()))) . "</p>";
  $output['content']["orgs"] = $form;

  // Settings form
  // ---------------------------------------------------------------------------

  $form = drupal_get_form('stanford_capx_forms_config_settings');
  $output["content"]["settingsform"] = $form;

  return $output;
}

/**
 * Callback url and utility function for running the organizations sync.
 * @return mixed
 *   redirects back to page user was on.
 */
function stanford_capx_organizations_sync() {
  try {
    watchdog('stanford_capx', 'Organization codes updated', array(), WATCHDOG_INFO);
    \CAPx\Drupal\Organizations\Orgs::syncOrganizations(TRUE);
  }
  catch(\Exception $e) {
    $message = t("ERROR! Could not update organization data. Reason given: %reason", array("%reason" => $e->getMessage()));
    watchdog('stanford_capx', $message, array(), WATCHDOG_ERROR);
    drupal_set_message($message, "error");
    drupal_goto(drupal_get_destination());
  }
}

/**
 * Stanford Capx help page menu callback.
 * @return string
 *   HTML representation of this page.
 */
function stanford_capx_admin_config_help() {
  $output = array();

  $content = "<h2>" . t("Getting started") . "</h2>";
  $content .= "<p>" . t("Importing content from CAP can be completed in 3 steps:") . "</p>";
  $content .= "<ol><li>";
  $content .= t("!connect - Connect to the CAP API using your authentication credentials.", array("!connect" => l(t("Connect"), "admin/config/capx/connect")));
  $content .= "</li><li>";
  $content .= t("!mapping - Create a Mapper that links CAP fields to your own fields.", array("!mapping" => l(t("Mapping"), "admin/config/capx/mapper")));
  $content .= "</li><li>";
  $content .= t("!import - Choose which profiles you would like to import.", array("!import" => l(t("Import"), "admin/config/capx/importer")));
  $content .= "</li></ol>";
  $content .= "<p>&nbsp;</p>";

  $content .= "<h2>" . t("Autentication credentials") . "</h2>";
  $content .= "<p>" . t("Authentication credentials allow you to connect to the CAP API and import content into your website.") . "</p>";
  $content .= "<p>" . t("To get authentication credentials, file a !helpsu to Administrative Applications/CAP Stanford Profiles.", array("!helpsu" => l(t("HelpSU request"), "https://www.stanford.edu/helpsu"))) . "</p>";
  $content .= "<p>&nbsp;</p>";

  $content .= "<h2>" . t("CAP data - Choosing fields") . "</h2>";
  $content .= "<p>" . t("If you need help determining which fields you need in your Mapper, you can use the !caplink. The schema displays all fields in CAP and where they are nested. Yes, it is a ton of data, we know...", array("!caplink" => l(t("CAP Data Schema"), "admin/config/capx/data-browser"))) . "</p>";
  $content .= "<p><strong>" . t("Common fields: ") . "</strong>";
  $content .= t("A list of common fields can be found on the sidebar of any !mappingpage.", array("!mappingpage" => l(t("mapper page"), "admin/config/capx/mapper/new"))) . "</p>";
  $content .= "<p>&nbsp;</p>";

  $content .= "<h2>" . t("Resources for developers") . "</h2>";
  $content .= "<p>" . t("Want to get involved in the development of the CAPx module? Check out the resources below:") . "</p>";
  $content .= "<ol><li>";
  $content .= l(t("CAPx Module UI documentation"), "http://www.github.com/SU-SWS/stanford_capx");
  $content .= "</li><li>";
  $content .= l(t("CAPx Module API documentation"), "http://www.github.com/SU-SWS/stanford_capx");
  $content .= "</li><li>";
  $content .= l(t("CAPx GitHub Repository"), "http://www.github.com/SU-SWS/stanford_capx");
  $content .= "</li><li>";
  $content .= l(t("CAPx Issue Queue"), "http://www.github.com/SU-SWS/stanford_capx");
  $content .= "</li><li>";
  $content .= l(t("CAPx Contribution help documentation"), "http://www.github.com/SU-SWS/stanford_capx");
  $content .= "</li><li>";
  $content .= l(t("CAP API documentation"), "http://www.github.com/SU-SWS/stanford_capx");
  $content .= "</li></ol>";

  $output['content']['textcontent']['#markup'] = $content;
  return $output;
}

/**
 * Stanford Capx mapper page menu callback.
 * @return string
 *   HTML representation of this page.
 */
function stanford_capx_admin_config_mapper() {
  $output = "<p>" . t("After you have connected to CAP, create a Mapping to link CAP&#8217;s fields with your fields") . ".</p>";
  $output .= "<p><b>" . l(t('Create new mapper'), 'admin/config/capx/mapper/new', array('attributes' => array('class' => 'new-button button btn'))) . "</b></p>";
  $output .= views_embed_view('capx_mappers');
  return $output;
}

/**
 * Stanford Capx create new mapper page menu callback.
 * @return string
 *   HTML representation of this page.
 */
function stanford_capx_admin_config_mapper_new() {
  $output = "";

  // Add some css.
  // drupal_add_css(drupal_get_path('module', 'stanford_capx') . "/css/stanford_capx.admin.css");
  $output = array();
  $output['#attached']['css'][] = drupal_get_path('module', 'stanford_capx') . "/css/stanford_capx.admin.css";

  // Data browser block.
  $block = block_load('stanford_capx', 'data_browser_launch');
  $output += _block_get_renderable_array(_block_render_blocks(array($block)));

  // Get and render the mapper form.
  $output += drupal_get_form('stanford_capx_mapper_form');


  return $output;
}

/**
 * Stanford Capx edit mapper page menu callback.
 * @return string
 *   HTML representation of this page.
 */
function stanford_capx_admin_config_mapper_edit($mapper_machine_name) {
  $output = array();
  $mapper = capx_cfe_load_by_machine_name($mapper_machine_name, 'mapper');

  if (!$mapper) {
    throw new Exception(t("Could not load a mapper with provided machine name. Please check url."));
  }

  // Add some css.
  // drupal_add_css(drupal_get_path('module', 'stanford_capx') . "/css/stanford_capx.admin.css");
  $output['#attached']['css'][] = drupal_get_path('module', 'stanford_capx') . "/css/stanford_capx.admin.css";

  // Data browser block.
  $block = block_load('stanford_capx', 'mapper_in_use');
  $output['content']['mapper_in_use'] = _block_get_renderable_array(_block_render_blocks(array($block)));

  // Data browser block.
  $block = block_load('stanford_capx', 'data_browser_launch');
  $output['content']['data_browser_launch'] = _block_get_renderable_array(_block_render_blocks(array($block)));

  // Render form.
  $output['content']['mapper_form'] = drupal_get_form('stanford_capx_mapper_form', $mapper);

  return $output;
}

/**
 * Importers page menu callback.
 * @return string
 *   HTML representation of this page.
 */
function stanford_capx_admin_config_import() {
  $output = "";
  $output .= "<p>" . t("After you have Mapping configured, create an Importer to chose which profiles you would like to import.") . "</p>";
  $output .= "<p>" . t("Importers allow you to chose CAP profiles in bulk by Organizations, or Workgroups, or SunetIDs.") . "</p>";
  $output .= "<p>" . l(t('Create new importer'), 'admin/config/capx/importer/new', array('attributes' => array('class' => 'new-button button btn'))) . "</p>";
  $output .= views_embed_view('capx_importers');

  return $output;
}

/**
 * Create new importer page menu callback.
 * @return string
 *   HTML representation of this page.
 */
function stanford_capx_admin_config_import_new() {
  // Get and render the importer form.
  return drupal_get_form('stanford_capx_importer_form');
}

/**
 * Edit importer configuration form page menu callback.
 * @return string
 *   HTML representation of this page.
 */
function stanford_capx_admin_config_import_edit($importer_machine_name) {
  $output = array();
  $importer = CAPxImporter::loadImporter($importer_machine_name);

  if (!$importer) {
    throw new Exception(t("Could not load an importer with provided machine name. Please check url."));
  }

  // The in use block.
  $output['content']["in_use_block"] = _block_get_renderable_array(_block_render_blocks(array(block_load('stanford_capx', 'importer_in_use'))));

  // Render form.
  $output['content']["importer_form"] = drupal_get_form('stanford_capx_importer_form', $importer);

  return $output;
}

/**
 * Runs a batch on a single importer.
 */
function stanford_capx_sync_importer($machine_name) {

  // Find by the name that was passed in.
  $importer = CAPxImporter::loadEntityImporter($machine_name);

  // Nothing to see here.
  if (!$importer) {
    $vars = array(
      '%name' => $machine_name,
      '!log' => l(t('log messages'), 'admin/reports/dblog'),
    );
    drupal_set_message(t('There was an issue loading the importer with %name machine name. Check !log.', $vars), 'error');
  }

  if (!$importer->valid()) {
    drupal_set_message(t('There was an issue running the importer. Settings did not validate. Please check the error logs for more information.'), 'error');
  }
  else {
    // Set the batch items.
    $importer->createBatch();

    // Run the show!
    batch_process(drupal_get_destination());
  }
}

/**
 * Runs a batch on a single orphanator.
 *
 * @param string $machine_name
 *   The machine name of the entity importer to run an orphan check on.
 *
 */
function stanford_capx_orphan_importer($machine_name) {

  // Sanity...
  $machine_name = check_plain($machine_name);

  // Get the orphanator object.
  $orphanator = CAPxImporter::getEntityOrphanator($machine_name);

  // Nothing to see must throw up.
  if (!$orphanator) {
    throw new Exception(t("Could not find an entity importer orphanator by that name."));
  }

  // Set the limit.
  $orphanator->setLimit(variable_get('stanford_capx_batch_limit', 100));

  // Set the batch items.
  $orphanator->batch();

  // Call batch.
  batch_process(drupal_get_destination());

}

/**
 * [stanford_capx_admin_config description]
 * @return [type] [description]
 */

function stanford_capx_debug() {
  $output = "";

  // $importers = CAPxImporter::loadAllImporters();
  // foreach ($importers as $key => $importer) {
  //   $entity_importer = $importer->getEntityImporter();
  //   $entity_importer->cron();
  // }


  // batch_process('admin/config/capx/profiles');

  $output .= "Hey!";
  return $output;
}

/**
 * Runs a batch on all importers.
 */
function stanford_capx_sync_all_importers() {

  $importers = CAPxImporter::getAllEntityImporters();

  foreach ($importers as $importer) {
    $importer->createBatch();
  }

  batch_process(drupal_get_destination());
}
