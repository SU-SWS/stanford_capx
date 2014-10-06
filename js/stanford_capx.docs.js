/**
 * @file
 * Renders the CAP Schema JSON tree.
 */

/**
 * Render the schema on page load.
 */
jQuery(document).ready(function () {
    jQuery('#capx-schema').tree({
        data: Drupal.settings.stanford_capx.schema,
        autoOpen: false,
        dragAndDrop: false
    });
});
