# Drush features export page variant #

This is a drush plugin for experienced Drupal developers.

It is a workaround for putting panel page variants in to another
feature than the "original" panel page.

This is intended as a temporary solution for getting the work done
while a better solution is incorporated to features and/or ctools.

Be sure to read the "Caveats" section below. And remember to have
backups and/or version control in place.

The plugin has been tested to work with both drush 4.5 and drush 5.x.

## Use case ##

You have a panel page that is either defined in a feature (with the
[features](http://drupal.org/project/features) module) and you want to
add a page variant of that page - except you would like to store it in
some other feature than where the original page was defined.

Currently features is unable to save your variant to another feature -
although it is very well capable of keeping it up to update once you
manage to have it saved elsewhere.

This drush plugin saves your panel page variant to another feature and
lets you get on with your work.

## Usage ##

Go ahead and create your page variant as usual.

Then add "my\_page\_handler_2" to "my-feature" with the help of
[drush](http://drupal.org/project/drush):
  
`$ drush features-export-page-variant my-feature my_page_handler_2`

or using the much shorter command alias, fepv:

`$ drush fepv my-feature my_page_handler_2`



## Finding the page handler name ##

You can either locate the page handler name in the URL:

![Find page_handler name from URL](https://github.com/DBCDK/drush-features-export-page-variant/raw/master/demo1.png)

or from the export tab of the variant - look for `$handler->name`:

![Find page_handler name from URL](https://github.com/DBCDK/drush-features-export-page-variant/raw/master/demo2.png)

## Caveats ##

*   The plugin will try to add a dependency on the feature that the
    "original" page is located in. But it does this by guessing!
  
    If the variant you want to export is named `my_page_handler_2` it
    will make a guess and decide that the original must be
    `my_page_handler` and add a dependency on the feature where that
    is located.
  
*   If you are more than one developer adding variants at the same
    time you might end up with more than one instance of a handler
    named `my_page_handler_2`.
  
    Be careful. Talk to each other and make sure your code is
    up-to-date and do a thorough review of the code before pushing to
    a common repository.
  
    If your end up with several page handlers with the same name you
    can edit the feature and renamed the page handler name manually.
    The name should occur one time in `my-feature.info` and two times
    in `my-feature.pages_default.inc`.

*   Trying to add a non-existing page variant will not produce any
    error messages or warnings.
  
    Even worse it will leave your feature in an inconsistent state.
    `drush feature-update my-feature` will probably bring it in to a
    consistent state again.

*   Adding a page variant that is already handled by another feature
    will not produce any error messages or warnings.
  
    Even worse it will leave your features/site in an inconsistent
    state.

*   You cannot remove a page variant from a feature again. At least
    not from the administrative interface or from drush.
  
    You will have to edit the feature module (as in change the source
    code) yourself to remove the page variant.
	
*   The feature you want to add the variant to must exist and must be
    recognizable as a feature. This means the feature module must have
    a `my_feature.features.inc` file. Creating a new feature from the
    UI and _not_ adding anything to the feature will _not_ add a
    `my_feature.features.inc` and will hence _not_ be recognized.

## The "real" solution ##

This is some of the issues related to integrating a similar solution
in features/ctools/panels:

* [Ability to set variant machine name in Panels UI](http://drupal.org/node/813754)
* [Exported panel variants as features are incompatible with each other](http://drupal.org/node/740074)

