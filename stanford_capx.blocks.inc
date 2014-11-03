<?php
/**
 * @file
 * @author [author] <[email]>
 *
 * A place to put your blocks.
 */

use CAPx\Drupal\Util\CAPxConnection;
use CAPx\Drupal\Util\CAPxImporter;

/**
 * Implements hook_block_info().
 */
function stanford_capx_block_info() {
  $block = array();

  $blocks['connection_status'] = array(
    'info' => t('Connection Status'),
    'cache' => DRUPAL_NO_CACHE,
    'properties' => array('administrative' => 1),
  );

  $blocks['data_browser_launch'] = array(
    'info' => t('Selecting fields'),
    'cache' => DRUPAL_NO_CACHE,
    'properties' => array('administrative' => 1),
    'region' => 'help',
    'visibility' => BLOCK_VISIBILITY_LISTED,
    'pages' => 'admin/config/capx/mapper/*',

  );

  return $blocks;
}

/**
 * Implements hook_block_view().
 */
function stanford_capx_block_view($delta = '') {
  $block = array();

  switch ($delta) {

    case 'connection_status':
      $block['subject'] = t('Connection Status');
      $block['content'] = stanford_capx_connection_status_block();
      break;

    case "data_browser_launch":
      $block['subject'] = t('Data Browser');
      $block['content'] = stanford_capx_data_browser_launch_block();
      break;

  }

  return $block;
}

/**
 * Connection status block content callback.
 *
 * Tests the API connection and outputs a text message.
 * @return string
 *   Block content.
 */
function stanford_capx_connection_status_block() {
  $content = '';
  $connection = CAPxConnection::testConnection();

  // Try to authenticate if not connected first try.
  if (!$connection->status) {
    CAPxConnection::renewConnectionToken();
    $connection = CAPxConnection::testConnection();
  }

  // We good now!
  if ($connection->status) {
    $total_profiles = 0;
    $last_update_human = 'Never';
    $last_update = 0;

    $importers = CAPxImporter::loadAllImporters();
    foreach ($importers as $importer) {
      $total_profiles += $importer->meta['count'];
      if ($last_update < $importer->meta['lastUpdate']) {
        $last_update = $importer->meta['lastUpdate'];;
        $last_update_human = $importer->meta['lastUpdateHuman'];
      }
    }

    $content .= '<h2>' . t('Connected!') . '</h2>';
    $content .= '<div class="cap-status-ok"><p>Congratulations, you have successfully authenticated with the CAP API.</p></div>';

    $content .= '<div class="profiles"><p>' . t('@count profiles imported from CAP.', array('@count' => $total_profiles)) . '</p></div>';
    $content .= '<div class="last-updated"><p>' . t('Last import: @date', array('@date' => $last_update_human)) . '</p></div>';
    // There is no need to show sync all link if there are no importers
    // configured.
    if (count($importers) > 0) {
      $link_vars = array(
        'attributes' => array('class' => array('button')),
        'query' => array('destination' => current_path()),
      );
      $link = l(t('Manually update all profiles'), 'admin/config/capx/importer/sync-all', $link_vars);
      $content .= '<div class="update-all"><p>' . $link . '</p></div>';
    }
  }
  // Something is not set correctly and we cannot connect.
  else {
    $content .= '<h2 class="error">' . t('Not Connected') . '</h2>';
    $vars = array('@code' => $connection->code, '@message' => $connection->message);
    $content .= '<div class="cap-status-bad"><p>' . t('The server responded with error code: @code and message: @message.', $vars) . '</p></div>';
  }

  return $content;
}

/**
 * Data browser launch block content callback.
 * Creates the blocks contents for the mapper edit/new page.
 * @return [type] [description]
 */
function stanford_capx_data_browser_launch_block() {
  $output = "";

  $output .= "<p>" . t("Having trouble finding the right field for mapping? Try viewing the data schema. We know... it is a lot. You can do it!") . "</p>";

  // Generate a link to the data browser, which opens in a new window.
  $browser_link = l(t('Open Data Schema'),
    'admin/config/capx/data-browser', array(
      'attributes' => array(
        'class' => 'btn button',
        'target' => '_blank'
      )
    ));
  $output .= "<p>" . $browser_link . "</p>";

  $output .= "<p>Here are some commonly used settings:</p>";
  $rows = array();
  $rows[] = array('Title', '$.displayName');
  $rows[] = array('Body', '$.bio.html');
  $rows[] = array('Profile picture', '$.profilePhotos.bigger');
  $rows[] = array('Email', '$.primaryContact.email');
  $rows[] = array('Last updated', '$.lastModified');
  $rows[] = array('-------------', "--------------");
  $rows[] = array('Cohort', '$.maintainers.*.title');
  $rows[] = array('Job title short', '$.shortTitle.label.text');
  $rows[] = array('Job title long', '$.longTitle.title');
  // $rows[] = array('Dissertation title', '');
  $rows[] = array('Degrees / education', '$.education.*.label.text');
  $rows[] = array('Title and department', '$.longTitle');
  // $rows[] = array('Faculty status', '');
  $rows[] = array('Fax', '$.primaryContact.fax');
  $rows[] = array('CV', '$.documents.cv.url');
  $rows[] = array('First name legal', '$.names.legal.firstName');
  $rows[] = array('First name preferred', '$.names.preferred.firstName');
  $rows[] = array('Last name legal', '$.names.legal.lastName');
  $rows[] = array('Last name preferred', '$.names.preferred.lastName');
  $rows[] = array('Middle name legal', '$.names.legal.middleName');
  $rows[] = array('Middle name preferred', '$.names.preferred.middleName');
  $rows[] = array('Graduation year', '$.education.*.yearIssued');
  $rows[] = array('Personal info links title', '$.internetLinks.*.label');
  $rows[] = array('Personal info links url', '$.internetLinks.*.url');
  $rows[] = array('Fields of interest', '$.professionalInterests.text');
  $rows[] = array('Mailing address', '$.primaryContact.address');
  $rows[] = array('Mailing address city', '$.primaryContact.city');
  $rows[] = array('Mailing address state', '$.primaryContact.state');
  $rows[] = array('Mailing address zip', '$.primaryContact.zip');
  // $rows[] = array('Office hours', '');
  $rows[] = array('Office', '$.primaryContact.officeName');
  $rows[] = array('Phone', '$.primaryContact.phoneNumbers.*');
  $rows[] = array('Staff type', '$.titles.*.type');
  $rows[] = array('Field of study', '$.education.*.fieldOfStudy');
  $rows[] = array('Type', '$.titles.*.type');
  $output .= theme('table', array('rows' => $rows));

  return $output;
}
