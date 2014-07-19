<?php
/**
 * @file
 * @author [author] <[email]>
 *
 * A place to put your blocks.
 */

use CAPx\Drupal\Util\CAPx;

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

  }

  return $block;
}

/**
 * [stanford_capx_connection_status_block description]
 * @return [type] [description]
 */
function stanford_capx_connection_status_block() {
  $content = "";
  $connection = CAPx::testConnection();

  // Try to authenticate if not connected first try.
  if (!$connection->status) {
    CAPx::renewConnectionToken();
    $connection = CAPx::testConnection();
  }

  // We good now!
  if ($connection->status) {
    $content .= "<h2>" . t('Connected!') . "</h2>";
    $content .= "<p>Congratulations, you have successfully authenticated with the CAP API.</p>";
  }
  // Something is not set correctly and we cannot connect.
  else {
    $content .= "<h2 class=\"error\">" . t('Not Connected') . "</h2>";
    $content .= "<p>" . t('The server responded with error code: ') . $connection->code . " " . t($connection->message) . ".</p>";
  }

  return $content;
}
