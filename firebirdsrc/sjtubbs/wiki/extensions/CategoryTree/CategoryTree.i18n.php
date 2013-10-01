<?php
/**
 * Internationalisation file for extension CategoryTree.
 *
 * @addtogroup Extensions
 * @author Daniel Kinzler, brightbyte.de
 * @copyright © 2006-2008 Daniel Kinzler
 * @license GNU General Public Licence 2.0 or later
 */

$messages = array();

/** English
 * @author Daniel Kinzler, brightbyte.de
 */
$messages['en'] = array(
	'categorytree'                  => 'Category tree',
	'categorytree-portlet'          => 'Categories',
	'categorytree-legend'           => 'Show category tree',
	'categorytree-desc'             => 'Dynamically navigate the [[Special:CategoryTree|category structure]]',
	'categorytree-header'           => 'Enter a category name to see its contents as a tree structure.
Note that this requires advanced JavaScript functionality known as AJAX.
If you have a very old browser, or have JavaScript disabled, it will not work.',

	'categorytree-category'         => 'Category:',
	'categorytree-go'               => 'Show tree',
	'categorytree-parents'          => 'Parents',

	'categorytree-mode-categories'  => 'categories only',
	'categorytree-mode-pages'       => 'pages except files',
	'categorytree-mode-all'         => 'all pages',

	'categorytree-collapse'         => 'collapse',
	'categorytree-expand'           => 'expand',
	'categorytree-collapse-bullet'  => '[<b>−</b>]', # do not translate or duplicate this message to other languages
	'categorytree-expand-bullet'    => '[<b>+</b>]', # do not translate or duplicate this message to other languages
	'categorytree-empty-bullet'     => '[<b>×</b>]', # do not translate or duplicate this message to other languages
	'categorytree-page-bullet'      => '&nbsp;', # do not translate or duplicate this message to other languages

	'categorytree-member-counts'    => 'contains {{PLURAL:$1|1 subcategory|$1 subcategories}}, {{PLURAL:$2|1 page|$2 pages}}, and {{PLURAL:$3|1 file|$3 files}}', # $1=subcategories, $2=subpages, $3=files, $4=total, $5=shown-in-tree
	'categorytree-member-num'    => '($5)', # do not translate or duplicate this message to other languages

	'categorytree-load'             => 'load',
	'categorytree-loading'          => 'loading…',
	'categorytree-nothing-found'    => 'nothing found',
	'categorytree-no-subcategories' => 'no subcategories',
	'categorytree-no-parent-categories' => 'no parent categories',
	'categorytree-no-pages'         => 'no pages or subcategories',
	'categorytree-not-found'        => 'Category <i>$1</i> not found',
	'categorytree-error'            => 'Problem loading data.',
	'categorytree-retry'            => 'Please wait a moment and try again.',
);

/** Message documentation (Message documentation)
 * @author Jon Harald Søby
 * @author Meno25
 * @author Raimond Spekking
 * @author Raymond
 * @author Александр Сигачёв
 * @author פוילישער
 */
$messages['qqq'] = array(
	'categorytree' => 'Title of [[Special:CategoryTree]]',
	'categorytree-portlet' => '{{Identical|Categories}}

Title for the CategoryPortlet, when shown in the side bar',
	'categorytree-legend' => 'Legend of the fieldset around the input form of [[Special:Categorytree]].',
	'categorytree-desc' => 'Short description of the CategoryTree extension, shown on [[Special:Version]]',
	'categorytree-header' => 'Header-text shown on [[Special:CategoryTree]]',
	'categorytree-category' => '{{Identical|Category}}

Label for the category input field on Special:CategoryTree',
	'categorytree-go' => 'Label for the submit button on [[Special:CategoryTree]]',
	'categorytree-parents' => 'Label for the list of parent categories on [[Special:CategoryTree]]',
	'categorytree-mode-categories' => 'Item for the mode choice on [[Special:CategoryTree]], indicating that only categories are listed',
	'categorytree-mode-pages' => 'Item for the mode choice on [[Special:CategoryTree]], indicating that no images are categories are listed',
	'categorytree-mode-all' => '{{Identical|All pages}}

Item for the mode choice on Special:CategoryTree, indicating that all pages are listed',
	'categorytree-collapse' => 'Tooltip for the "collapse" button',
	'categorytree-expand' => 'Tooltip for the "expand" button',
	'categorytree-collapse-bullet' => 'Visible form of the "collapse" button (symbolic, do not translate)', # do not translate or duplicate this message to other languages
	'categorytree-expand-bullet' => 'Visible form of the "expand" button (symbolic, do not translate)', # do not translate or duplicate this message to other languages
	'categorytree-empty-bullet' => 'Visible form of the "expand" button for empty branches (symbolic, do not translate)', # do not translate or duplicate this message to other languages
	'categorytree-page-bullet' => 'Visible bullet for non-category items in the tree (symbolic, do not translate)', # do not translate or duplicate this message to other languages
	'categorytree-member-counts' => 'Tooltip showing a detailed summary of subcategory member counts. Parameters: 
			$1 = number of subcategories, 
			$2 = number of pages (without subcategories and files), 
			$3 = number of files, 
			$4 = total number of members, 
			$5 = members to be shown in the tree, depending on mode. 
			Use with { {PLURAL} }',
	'categorytree-member-num' => 'Short summary of subcategory member counts, to be shown inline.
			Usually numeric, translation should not be neccessary. Parameters: 
			$1 = number of subcategories, 
			$2 = number of pages (without subcategories and files), 
			$3 = number of files, 
			$4 = total number of members, 
			$5 = members to be shown in the tree, depending on mode. 
			Use with { {PLURAL} }', # do not translate or duplicate this message to other languages
	'categorytree-load' => '{{Identical|Load}}

Tooltip for the "expend" button, if the content was not yet loaded',
	'categorytree-loading' => '{{Identical|Loading}}

Status message shown while loading content',
	'categorytree-nothing-found' => 'Indicates items with matching criteria have been found',
	'categorytree-no-subcategories' => 'Indicates that there are no subcategories to be shown',
	'categorytree-no-parent-categories' => 'Indicates that there are no parent categories to be shown',
	'categorytree-no-pages' => 'Indicates that there are no pages or subcategories to be shown',
	'categorytree-not-found' => 'Indicates that the given category ($1) was not found',
	'categorytree-error' => "Indicates that an error has occurred while loading the node's content",
	'categorytree-retry' => 'Instruction to try again later',
);

/** Simplified Chinese (?中文(简体)?)
 * @author Shinjiman
 */
$messages['zh-hans'] = array(
	'categorytree' => '分类树',
	'categorytree-portlet' => '分类',
	'categorytree-legend' => '显示分类树',
	'categorytree-desc' => '以AJAX技术显示[[Special:CategoryTree|分类结构]]',
	'categorytree-header' => '在此可以查询以分类的树形结构。
注意： 本特殊页面使用AJAX技术。
如果您的浏览器非常老旧，或者是关闭了JavaScript，本页面将会无法正常运作。',
	'categorytree-category' => '分类',
	'categorytree-go' => '显示树形结构',
	'categorytree-parents' => '上级分类',
	'categorytree-mode-categories' => '只显示分类',
	'categorytree-mode-pages' => '除去图像页面',
	'categorytree-mode-all' => '所有页面',
	'categorytree-collapse' => '折叠',
	'categorytree-expand' => '展开',
	'categorytree-member-counts' => '含有$1个子分类、$2个页面和$3个文件',
	'categorytree-load' => '装载',
	'categorytree-loading' => '装载中…',
	'categorytree-nothing-found' => '搜索结果为空',
	'categorytree-no-subcategories' => '没有子分类',
	'categorytree-no-parent-categories' => '没有父分类',
	'categorytree-no-pages' => '没有文章或是子分类。',
	'categorytree-not-found' => '找不到分类<i>$1</i>',
	'categorytree-error' => '载入数据时发生错误。',
	'categorytree-retry' => '请稍候一会，然后再试。',
);
