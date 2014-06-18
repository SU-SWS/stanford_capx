# $Id$
ErrorDocument 401 <?php print base_path(); ?>user

AuthType WebAuth
WebAuthOptional off
<?php foreach($ldap_vars as $ldapvar): ?>
WebAuthLdapAttribute <?php print $ldapvar . "\n"; ?>
<?php endforeach; ?>

<?php if ($rewrite_url): ?>
RewriteEngine on
RewriteCond %{REQUEST_FILENAME} !-f
<?php print $rewrite_url; ?>
<?php endif; ?>

# Auto-generated below this line. Changes will be overwritten.
<?php if ($require_valid_user): ?>
require valid-user
<?php else: ?>
<?php foreach($users as $u): ?>
require user <?php print $u; ?>

<?php endforeach; ?>

<?php foreach($privgroups as $group): ?>
require privgroup <?php print $group; ?>

<?php endforeach; ?>
<?php endif; ?>

<?php foreach ($groups as $group): ?>
WebAuthLdapPrivgroup <?php print $group . "\n"; ?>
<?php endforeach; ?>
