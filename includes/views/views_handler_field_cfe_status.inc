<?php

/**
 * @file
 * Definition of stanford_capx_handler_field_configuration_settings.
 */

/**
 * Field handler to create a link to CAP profiles.
 *
 * @ingroup views_field_handlers
 */
class views_handler_field_cfe_status extends views_handler_field_numeric {

  /**
   * Translate the bitcode to a text status for the human.
   */
  function render($values) {
    $human = array(
      ENTITY_CUSTOM => t('Custom'),
      ENTITY_IN_DB => t('In DB'),
      ENTITY_FIXED => t('Fixed'),
      ENTITY_IN_CODE => t('In Code'),
      ENTITY_OVERRIDDEN => t('Overridden'),
    );
    return isset($human[$values->capx_cfe_status]) ? $human[$values->capx_cfe_status] : $human[1];
  }

}
