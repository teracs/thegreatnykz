<?php
/**
 * Internationalisation file for Cite extension.
 *
 * @addtogroup Extensions
*/

$messages = array();

$messages['en'] = array(
	'cite_desc'                      => 'Adds <nowiki><ref[ name=id]></nowiki> and <nowiki><references/></nowiki> tags, for citations',
	/*
		Debug and errors
	*/
	# Internal errors
	'cite_croak'                     => 'Cite died; $1: $2',
	'cite_error_key_str_invalid'     => 'Internal error;
invalid $str and/or $key.
This should never occur.',
	'cite_error_stack_invalid_input' => 'Internal error;
invalid stack key.
This should never occur.',

	# User errors
	'cite_error'                                     => 'Cite error: $1',
	'cite_error_ref_numeric_key'                     => 'Invalid <code>&lt;ref&gt;</code> tag;
name cannot be a simple integer. Use a descriptive title',
	'cite_error_ref_no_key'                          => 'Invalid <code>&lt;ref&gt;</code> tag;
refs with no content must have a name',
	'cite_error_ref_too_many_keys'                   => 'Invalid <code>&lt;ref&gt;</code> tag;
invalid names, e.g. too many',
	'cite_error_ref_no_input'                        => 'Invalid <code>&lt;ref&gt;</code> tag;
refs with no name must have content',
	'cite_error_references_invalid_input'            => 'Invalid <code>&lt;references&gt;</code> tag;
no input is allowed. Use <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters'       => 'Invalid <code>&lt;references&gt;</code> tag;
no parameters are allowed.
Use <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters_group' => 'Invalid <code>&lt;references&gt;</code> tag;
parameter "group" is allowed only.
Use <code>&lt;references /&gt;</code>, or <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label'        => 'Ran out of custom backlink labels.
Define more in the <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki> message',
	'cite_error_references_no_text'                  => 'Invalid <code>&lt;ref&gt;</code> tag;
no text was provided for refs named <code>$1</code>',
	'cite_error_included_ref'                        => 'Closing &lt;/ref&gt; missing for &lt;ref&gt; tag',

	/*
	   Output formatting
	*/
	'cite_reference_link_key_with_num' => '$1_$2',
	# Ids produced by <ref>
	'cite_reference_link_prefix'       => 'cite_ref-',
	'cite_reference_link_suffix'       => '',
	# Ids produced by <references>
	'cite_references_link_prefix'      => 'cite_note-',
	'cite_references_link_suffix'      => '',

	'cite_reference_link'                              => '<sup id="$1" class="reference">[[#$2|<nowiki>[</nowiki>$3<nowiki>]</nowiki>]]</sup>',
	'cite_references_link_one'                         => '<li id="$1">[[#$2|↑]] $3</li>',
	'cite_references_link_many'                        => '<li id="$1">↑ $2 $3</li>',
	'cite_references_link_many_format'                 => '<sup>[[#$1|$2]]</sup>',
	# An item from this set is passed as $3 in the message above
	'cite_references_link_many_format_backlink_labels' => 'a b c d e f g h i j k l m n o p q r s t u v w x y z aa ab ac ad ae af ag ah ai aj ak al am an ao ap aq ar as at au av aw ax ay az ba bb bc bd be bf bg bh bi bj bk bl bm bn bo bp bq br bs bt bu bv bw bx by bz ca cb cc cd ce cf cg ch ci cj ck cl cm cn co cp cq cr cs ct cu cv cw cx cy cz da db dc dd de df dg dh di dj dk dl dm dn do dp dq dr ds dt du dv dw dx dy dz ea eb ec ed ee ef eg eh ei ej ek el em en eo ep eq er es et eu ev ew ex ey ez fa fb fc fd fe ff fg fh fi fj fk fl fm fn fo fp fq fr fs ft fu fv fw fx fy fz ga gb gc gd ge gf gg gh gi gj gk gl gm gn go gp gq gr gs gt gu gv gw gx gy gz ha hb hc hd he hf hg hh hi hj hk hl hm hn ho hp hq hr hs ht hu hv hw hx hy hz ia ib ic id ie if ig ih ii ij ik il im in io ip iq ir is it iu iv iw ix iy iz ja jb jc jd je jf jg jh ji jj jk jl jm jn jo jp jq jr js jt ju jv jw jx jy jz ka kb kc kd ke kf kg kh ki kj kk kl km kn ko kp kq kr ks kt ku kv kw kx ky kz la lb lc ld le lf lg lh li lj lk ll lm ln lo lp lq lr ls lt lu lv lw lx ly lz ma mb mc md me mf mg mh mi mj mk ml mm mn mo mp mq mr ms mt mu mv mw mx my mz na nb nc nd ne nf ng nh ni nj nk nl nm nn no np nq nr ns nt nu nv nw nx ny nz oa ob oc od oe of og oh oi oj ok ol om on oo op oq or os ot ou ov ow ox oy oz pa pb pc pd pe pf pg ph pi pj pk pl pm pn po pp pq pr ps pt pu pv pw px py pz qa qb qc qd qe qf qg qh qi qj qk ql qm qn qo qp qq qr qs qt qu qv qw qx qy qz ra rb rc rd re rf rg rh ri rj rk rl rm rn ro rp rq rr rs rt ru rv rw rx ry rz sa sb sc sd se sf sg sh si sj sk sl sm sn so sp sq sr ss st su sv sw sx sy sz ta tb tc td te tf tg th ti tj tk tl tm tn to tp tq tr ts tt tu tv tw tx ty tz ua ub uc ud ue uf ug uh ui uj uk ul um un uo up uq ur us ut uu uv uw ux uy uz va vb vc vd ve vf vg vh vi vj vk vl vm vn vo vp vq vr vs vt vu vv vw vx vy vz wa wb wc wd we wf wg wh wi wj wk wl wm wn wo wp wq wr ws wt wu wv ww wx wy wz xa xb xc xd xe xf xg xh xi xj xk xl xm xn xo xp xq xr xs xt xu xv xw xx xy xz ya yb yc yd ye yf yg yh yi yj yk yl ym yn yo yp yq yr ys yt yu yv yw yx yy yz za zb zc zd ze zf zg zh zi zj zk zl zm zn zo zp zq zr zs zt zu zv zw zx zy zz',
	'cite_references_link_many_sep'                    => " ",
	'cite_references_link_many_and'                    => " ",

	# Although I could just use # instead of <li> above and nothing here that
	# will break on input that contains linebreaks
	'cite_references_prefix' => '<ol class="references">',
	'cite_references_suffix' => '</ol>',
);

/** Message documentation (Message documentation)
 * @author Lejonel
 * @author Purodha
 * @author Raimond Spekking
 * @author Raymond
 * @author Siebrand
 */
$messages['qqq'] = array(
	'cite_desc' => 'Extension description for cite. Shown in [[Special:Version]]. Do not translate or change tag names.',
	'cite_error' => 'Cite extension. This is used when there are errors in ref or references tags. The parameter $1 is an error message.',
	'cite_error_ref_numeric_key' => 'Cite extension. Error message shown if the name of a ref tag only contains digits. Examples that cause this error are <code>&lt;ref name="123" /&gt;</code> or <code>&lt;ref name="456"&gt;input&lt;/ref&gt;</code>',
	'cite_error_ref_no_key' => 'Cite extension. Error message shown when ref tags without any content (that is <code>&lt;ref/&gt;</code>) are used without a name.',
	'cite_error_ref_too_many_keys' => 'Cite extension. Error message shown when ref tags has parameters other than name. Examples that cause this error are <code>&lt;ref name="name" notname="value" /&gt;</code> or <code>&lt;ref notname="value" &gt;input&lt;ref&gt;</code>',
	'cite_error_ref_no_input' => 'Cite extension. Error message shown when ref tags without names have no content. An example that cause this error is <code>&lt;ref&gt;&lt;/ref&gt;</code>',
	'cite_error_references_invalid_input' => 'Cite extension. Error message shown if input is used in the references tag. An example that will cause this error is <code>&lt;references&gt;Some input&lt;/references&gt;</code>',
	'cite_error_references_invalid_parameters' => 'Cite extension. Error message shown when parmeters are used in the references tag. An example that cause this error is <code>&lt;references someparameter="value" /&gt;</code>',
	'cite_error_references_invalid_parameters_group' => 'Cite extension. Error message shown when unknown parameters are used in the references tag. An example that cause this error is <tt><nowiki><references someparameter="value" /></nowiki></tt>',
	'cite_error_references_no_backlink_label' => 'Cite extension. Error message shown in the references tag when the same name is used for too many ref tags. Too many in this case is more than there are backlink labels defined in [[MediaWiki:Cite references link many format backlink labels]].

It is not possible to make a clickable link to this message. "nowiki" is mandatory around [[MediaWiki:Cite references link many format backlink labels]].',
	'cite_error_references_no_text' => 'Cite extension. This error occurs when the tag <code>&lt;ref name="something" /&gt;</code> is used with the name-option specified and no other tag specifies a cite-text for this name.',
	'cite_error_included_ref' => 'Error message shown if the <tt>&lt;ref&gt;</tt> tag is unbalanced, that means a <tt>&lt;ref&gt;</tt> is not followed by a <tt>&lt;/ref&gt;</tt>',
);

/** Afrikaans (Afrikaans)
 * @author Naudefj
 */
$messages['af'] = array(
	'cite_desc' => 'Maak <nowiki><ref[ name=id]></nowiki> en <nowiki><references/></nowiki> etikette beskikbaar vir sitasie.',
	'cite_croak' => 'Probleem met Cite; $1: $2',
	'cite_error' => 'Citefout: $1',
);

/** Aragonese (Aragonés)
 * @author Juanpabl
 */
$messages['an'] = array(
	'cite_desc' => 'Adibe as etiquetas <nowiki><ref[ name=id]></nowiki> y <nowiki><references/></nowiki> ta fer zitas',
	'cite_croak' => 'Zita corrompita; $1: $2',
	'cite_error_key_str_invalid' => 'Error interna; $str y/u $key no conforme(s). Isto no abría d\'escaizer nunca.',
	'cite_error_stack_invalid_input' => "Error interna; clau de pila no conforme. Isto no abría d'escaizer nunca.",
	'cite_error' => 'Error en a zita: $1',
	'cite_error_ref_numeric_key' => "Etiqueta <code>&lt;ref&gt;</code> incorreuta; o nombre d'a etiqueta no puede estar un numero entero, faiga serbir un títol descriptibo",
	'cite_error_ref_no_key' => 'Etiqueta <code>&lt;ref&gt;</code> incorreuta; as referenzias sin de conteniu han de tener un nombre',
	'cite_error_ref_too_many_keys' => 'Etiqueta <code>&lt;ref&gt;</code> incorreuta; nombres de parametros incorreutos.',
	'cite_error_ref_no_input' => 'Etiqueta <code>&lt;ref&gt;</code> incorreuta; as referenzias sin nombre no han de tener conteniu',
	'cite_error_references_invalid_input' => 'Etiqueta <code>&lt;references&gt;</code> incorreuta; no se premite emplegar input en a etiqueta, faiga serbir <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'Etiqueta <code>&lt;references&gt;</code> incorreuta; no se premiten parametros, faiga serbir <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters_group' => 'Etiqueta <code>&lt;references&gt;</code> no conforme;
nomás se premite o parametro "group".
Faiga serbir <code>&lt;references /&gt;</code>, u <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => 'Ya no quedan etiquetas backlink presonalizatas, defina más en o mensache <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki>',
	'cite_error_references_no_text' => "Etiqueta <code>&lt;ref&gt;</code> incorreuta; no ha escrito garra testo t'as referenzias nombratas <code>$1</code>",
	'cite_error_included_ref' => 'Zarrando &lt;/ref&gt; falta una etiqueta &lt;ref&gt;',
);

/** Arabic (العربية)
 * @author Meno25
 */
$messages['ar'] = array(
	'cite_desc' => 'يضيف وسوم <nowiki><ref[ name=id]></nowiki> و <nowiki><references/></nowiki> ، للاستشهادات',
	'cite_croak' => 'الاستشهاد مات؛ $1: $2',
	'cite_error_key_str_invalid' => 'خطأ داخلي؛
$str و/أو $key غير صحيح.
هذا لا يجب أن يحدث أبدا.',
	'cite_error_stack_invalid_input' => 'خطأ داخلي؛
مفتاح ستاك غير صحيح.
هذا لا يجب أن يحدث أبدا.',
	'cite_error' => 'خطأ استشهاد: $1',
	'cite_error_ref_numeric_key' => 'وسم <code>&lt;ref&gt;</code> غير صحيح؛
الاسم لا يمكن أن يكون عددا صحيحا بسيطا. استخدم عنوانا وصفيا',
	'cite_error_ref_no_key' => 'وسم <code>&lt;ref&gt;</code> غير صحيح؛
المراجع غير ذات المحتوى يجب أن تمتلك اسما',
	'cite_error_ref_too_many_keys' => 'وسم <code>&lt;ref&gt;</code> غير صحيح؛
أسماء غير صحيحة، على سبيل المثال كثيرة جدا',
	'cite_error_ref_no_input' => 'وسم <code>&lt;ref&gt;</code> غير صحيح؛
المراجع غير ذات الاسم يجب أن تمتلك محتوى',
	'cite_error_references_invalid_input' => 'وسم <code>&lt;references&gt;</code> غير صحيح؛
لا مدخل مسموح به. استخدم <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'وسم <code>&lt;references&gt;</code> غير صحيح؛
لا محددات مسموح بها.
استخدم <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters_group' => 'وسم <code>&lt;references&gt;</code> غير صحيح؛
المحدد "group" فقط مسموح به.
استخدم <code>&lt;references /&gt;</code>، أو <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => 'نفدت علامات الوصلات الراجعة المخصصة.
عرف المزيد في رسالة <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki>',
	'cite_error_references_no_text' => 'وسم <code>&lt;ref&gt;</code> غير صحيح؛
لا نص تم توفيره للمراجع المسماة <code>$1</code>',
	'cite_error_included_ref' => 'إغلاق &lt;/ref&gt; مفقود لوسم &lt;ref&gt;',
	'cite_references_link_many_format_backlink_labels' => 'أ ب ت ث ج ح خ د ذ ر ز س ش ص ض ط ظ ع غ ف ق ك ل م ن ه و ي أأ أب أت أث أج أح أخ أد أذ أر أز أس أش أص أض أط أظ أع أغ أف أق أك أل أم أن أه أو أي بأ بب بت بث بج بح بخ بد بذ بر بز بس بش بص بض بط بظ بع بغ بف بق بك بل بم بن به بو بي تأ تب تت تث تج تح تخ تد تذ تر تز تس تش تص تض تط تظ تع تغ تف تق تك تل تم تن ته تو تي ثأ ثب ثت ثث ثج ثح ثخ ثد ثذ ثر ثز ثس ثش ثص ثض ثط ثظ ثع ثغ ثف ثق ثك ثل ثم ثن ثه ثو ثي جأ جب جت جث جج جح جخ جد جذ جر جز جس جش جص جض جط جظ جع جغ جف جق جك جل جم جن جه جو جي حأ حب حت حث حج حح حخ حد حذ حر حز حس حش حص حض حط حظ حع حغ حف حق حك حل حم حن حه حو حي خأ خب خت خث خج خح خخ خد خذ خر خز خس خش خص خض خط خظ خع خغ خف خق خك خل خم خن خه خو خي دأ دب دت دث دج دح دخ دد دذ در دز دس دش دص دض دط دظ دع دغ دف دق دك دل دم دن ده دو دي ذأ ذب ذت ذث ذج ذح ذخ ذد ذذ ذر ذز ذس ذش ذص ذض ذط ذظ ذع ذغ ذف ذق ذك ذل ذم ذن ذه ذو ذي رأ رب رت رث رج رح رخ رد رذ رر رز رس رش رص رض رط رظ رع رغ رف رق رك رل رم رن ره رو ري زأ زب زت زث زج زح زخ زد زذ زر زز زس زش زص زض زط زظ زع زغ زف زق زك زل زم زن زه زو زي سأ سب ست سث سج سح سخ سد سذ سر سز سس سش سص سض سط سظ سع سغ سف سق سك سل سم سن سه سو سي شأ شب شت شث شج شح شخ شد شذ شر شز شس شش شص شض شط شظ شع شغ شف شق شك شل شم شن شه شو شي صأ صب صت صث صج صح صخ صد صذ صر صز صس صش صص صض صط صظ صع صغ صف صق صك صل صم صن صه صو صي ضأ ضب ضت ضث ضج ضح ضخ ضد ضذ ضر ضز ضس ضش ضص ضض ضط ضظ ضع ضغ ضف ضق ضك ضل ضم ضن ضه ضو ضي طأ طب طت طث طج طح طخ طد طذ طر طز طس طش طص طض طط طظ طع طغ طف طق طك طل طم طن طه طو طي ظأ ظب ظت ظث ظج ظح ظخ ظد ظذ ظر ظز ظس ظش ظص ظض ظط ظظ ظع ظغ ظف ظق ظك ظل ظم ظن ظه ظو ظي عأ عب عت عث عج عح عخ عد عذ عر عز عس عش عص عض عط عظ عع عغ عف عق عك عل عم عن عه عو عي غأ غب غت غث غج غح غخ غد غذ غر غز غس غش غص غض غط غظ غع غغ غف غق غك غل غم غن غه غو غي فأ فب فت فث فج فح فخ فد فذ فر فز فس فش فص فض فط فظ فع فغ فف فق فك فل فم فن فه فو في قأ قب قت قث قج قح قخ قد قذ قر قز قس قش قص قض قط قظ قع قغ قف قق قك قل قم قن قه قو قي كأ كب كت كث كج كح كخ كد كذ كر كز كس كش كص كض كط كظ كع كغ كف كق كك كل كم كن كه كو كي لأ لب لت لث لج لح لخ لد لذ لر لز لس لش لص لض لط لظ لع لغ لف لق لك لل لم لن له لو لي مأ مب مت مث مج مح مخ مد مذ مر مز مس مش مص مض مط مظ مع مغ مف مق مك مل مم من مه مو مي نأ نب نت نث نج نح نخ ند نذ نر نز نس نش نص نض نط نظ نع نغ نف نق نك نل نم نن نه نو ني هأ هب هت هث هج هح هخ هد هذ هر هز هس هش هص هض هط هظ هع هغ هف هق هك هل هم هن هه هو هي وأ وب وت وث وج وح وخ ود وذ ور وز وس وش وص وض وط وظ وع وغ وف وق وك ول وم ون وه وو وي يأ يب يت يث يج يح يخ يد يذ ير يز يس يش يص يض يط يظ يع يغ يف يق يك يل يم ين يه يو يي',
);

/** Egyptian Spoken Arabic (مصرى)
 * @author Ghaly
 * @author Meno25
 * @author Ramsis II
 */
$messages['arz'] = array(
	'cite_desc' => 'بيضيف التاجز <nowiki><ref[ name=id]></nowiki> و <nowiki><references/></nowiki> ، للاستشهاد',
	'cite_croak' => 'المرجع مات; $1: $2',
	'cite_error_key_str_invalid' => 'غلط داخلى؛
$str و/أو $key غلط.
ده لازم مايحصلش ابدا.',
	'cite_error_stack_invalid_input' => 'غلط داخلي؛
مفتاح ستاك مش صحيح.
ده لازم مايحصلش ابدا',
	'cite_error' => 'المرجع غلط: $1',
	'cite_error_ref_numeric_key' => 'التاج <code>&lt;ref&gt;</code> مش صحيح؛
الاسم ماينفعش يكون عدد صحيح بسيط. استخدم عنوان بيوصف',
	'cite_error_ref_no_key' => 'التاج <code>&lt;ref&gt;</code> مش صحيح؛
المراجع اللى من غير محتوى لازميكون ليها اسم',
	'cite_error_ref_too_many_keys' => 'التاج <code>&lt;ref&gt;</code> مش صحيح؛
أسامى مش صحيحة، يعنى مثلا: كتير قوي',
	'cite_error_ref_no_input' => 'تاج <code>&lt;ref&gt;</code> مش صحيح؛
المراجع اللى من غير اسم لازم يكون ليها محتوى',
	'cite_error_references_invalid_input' => 'مش صحيح <code>&lt;references&gt;</code> تاج;
مافيش مدخل مسموح بيه. استخدم <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'مش صحيح <code>&lt;references&gt;</code> تاج;
مافيش محددات مسموح بيها.
استخدم <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters_group' => 'مش صحيح <code>&lt;references&gt;</code> تاج;
محدد "group" مسموح بيه بس.
استخدم <code>&lt;references /&gt;</code>, or <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => 'علامات الوصلات الراجعة المخصصة خلصت.
عرف اكتر فى رسالة <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki>',
	'cite_error_references_no_text' => 'مش صحيح <code>&lt;ref&gt;</code> تاج;
مافيش نص متوافر فى المراجع اللى اسمها<code>$1</code>',
	'cite_error_included_ref' => 'إغلاق &lt;/ref&gt; مفقود لوسم &lt;ref&gt;',
	'cite_references_link_many_format_backlink_labels' => 'أ ب ت ث ج ح خ د ذ ر ز س ش ص ض ط ظ ع غ ف ق ك ل م ن ه و ى أأ أب أت أث أج أح أخ أد أذ أر أز أس أش أص أض أط أظ أع أغ أف أق أك أل أم أن أه أو أى بأ بب بت بث بج بح بخ بد بذ بر بز بس بش بص بض بط بظ بع بغ بف بق بك بل بم بن به بو بى تأ تب تت تث تج تح تخ تد تذ تر تز تس تش تص تض تط تظ تع تغ تف تق تك تل تم تن ته تو تى ثأ ثب ثت ثث ثج ثح ثخ ثد ثذ ثر ثز ثس ثش ثص ثض ثط ثظ ثع ثغ ثف ثق ثك ثل ثم ثن ثه ثو ثى جأ جب جت جث جج جح جخ جد جذ جر جز جس جش جص جض جط جظ جع جغ جف جق جك جل جم جن جه جو جى حأ حب حت حث حج حح حخ حد حذ حر حز حس حش حص حض حط حظ حع حغ حف حق حك حل حم حن حه حو حى خأ خب خت خث خج خح خخ خد خذ خر خز خس خش خص خض خط خظ خع خغ خف خق خك خل خم خن خه خو خى دأ دب دت دث دج دح دخ دد دذ در دز دس دش دص دض دط دظ دع دغ دف دق دك دل دم دن ده دو دى ذأ ذب ذت ذث ذج ذح ذخ ذد ذذ ذر ذز ذس ذش ذص ذض ذط ذظ ذع ذغ ذف ذق ذك ذل ذم ذن ذه ذو ذى رأ رب رت رث رج رح رخ رد رذ رر رز رس رش رص رض رط رظ رع رغ رف رق رك رل رم رن ره رو رى زأ زب زت زث زج زح زخ زد زذ زر زز زس زش زص زض زط زظ زع زغ زف زق زك زل زم زن زه زو زى سأ سب ست سث سج سح سخ سد سذ سر سز سس سش سص سض سط سظ سع سغ سف سق سك سل سم سن سه سو سى شأ شب شت شث شج شح شخ شد شذ شر شز شس شش شص شض شط شظ شع شغ شف شق شك شل شم شن شه شو شى صأ صب صت صث صج صح صخ صد صذ صر صز صس صش صص صض صط صظ صع صغ صف صق صك صل صم صن صه صو صى ضأ ضب ضت ضث ضج ضح ضخ ضد ضذ ضر ضز ضس ضش ضص ضض ضط ضظ ضع ضغ ضف ضق ضك ضل ضم ضن ضه ضو ضى طأ طب طت طث طج طح طخ طد طذ طر طز طس طش طص طض طط طظ طع طغ طف طق طك طل طم طن طه طو طى ظأ ظب ظت ظث ظج ظح ظخ ظد ظذ ظر ظز ظس ظش ظص ظض ظط ظظ ظع ظغ ظف ظق ظك ظل ظم ظن ظه ظو ظى عأ عب عت عث عج عح عخ عد عذ عر عز عس عش عص عض عط عظ عع عغ عف عق عك عل عم عن عه عو عى غأ غب غت غث غج غح غخ غد غذ غر غز غس غش غص غض غط غظ غع غغ غف غق غك غل غم غن غه غو غى فأ فب فت فث فج فح فخ فد فذ فر فز فس فش فص فض فط فظ فع فغ فف فق فك فل فم فن فه فو فى قأ قب قت قث قج قح قخ قد قذ قر قز قس قش قص قض قط قظ قع قغ قف قق قك قل قم قن قه قو قى كأ كب كت كث كج كح كخ كد كذ كر كز كس كش كص كض كط كظ كع كغ كف كق كك كل كم كن كه كو كى لأ لب لت لث لج لح لخ لد لذ لر لز لس لش لص لض لط لظ لع لغ لف لق لك لل لم لن له لو لى مأ مب مت مث مج مح مخ مد مذ مر مز مس مش مص مض مط مظ مع مغ مف مق مك مل مم من مه مو مى نأ نب نت نث نج نح نخ ند نذ نر نز نس نش نص نض نط نظ نع نغ نف نق نك نل نم نن نه نو نى هأ هب هت هث هج هح هخ هد هذ هر هز هس هش هص هض هط هظ هع هغ هف هق هك هل هم هن هه هو هى وأ وب وت وث وج وح وخ ود وذ ور وز وس وش وص وض وط وظ وع وغ وف وق وك ول وم ون وه وو وى يأ يب يت يث يج يح يخ يد يذ ير يز يس يش يص يض يط يظ يع يغ يف يق يك يل يم ين يه يو يى',
);

/** Asturian (Asturianu)
 * @author Esbardu
 */
$messages['ast'] = array(
	'cite_desc' => 'Añade les etiquetes <nowiki><ref[ name=id]></nowiki> y <nowiki><references/></nowiki> pa les cites',
	'cite_croak' => 'Cita corrompida; $1: $2',
	'cite_error_key_str_invalid' => 'Error internu; $str y/o $key non válidos.  Esto nun habría asoceder nunca.',
	'cite_error_stack_invalid_input' => 'Error internu; clave de pila non válida. Esto nun habría asoceder nunca.',
	'cite_error' => 'Error de cita: $1',
	'cite_error_ref_numeric_key' => 'Etiqueta <code>&lt;ref&gt;</code> non válida; el nome nun pue ser un enteru simple, usa un títulu descriptivu',
	'cite_error_ref_no_key' => 'Etiqueta <code>&lt;ref&gt;</code> non válida; les referencies ensin conteníu han tener un nome',
	'cite_error_ref_too_many_keys' => 'Etiqueta <code>&lt;ref&gt;</code> non válida; nomes non válidos (p.ex. demasiaos)',
	'cite_error_ref_no_input' => 'Etiqueta <code>&lt;ref&gt;</code> non válida; les referencies ensin nome han tener conteníu',
	'cite_error_references_invalid_input' => 'Etiqueta <code>&lt;references&gt;</code> non válida; nun se permiten entraes, usa
<code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'Etiqueta <code>&lt;references&gt;</code> non válida; nun se permiten parámetros, usa <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters_group' => 'Etiqueta <code>&lt;references&gt;</code> non válida;
namái se permite\'l parámetru "group".
Usa <code>&lt;references /&gt;</code>, o bien <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => 'Etiquetes personalizaes agotaes.
Defini más nel mensaxe <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki>',
	'cite_error_references_no_text' => 'Etiqueta <code>&lt;ref&gt;</code> non válida; nun se conseñó testu pa les referencies nomaes <code>$1</code>',
	'cite_error_included_ref' => 'Falta &lt;/ref&gt; pa la etiqueta &lt;ref&gt;',
	'cite_references_link_many_format_backlink_labels' => 'a b c d e f g h i j k l m n ñ o p q r s t u v w x y z',
);

/** Southern Balochi (بلوچی مکرانی)
 * @author Mostafadaneshvar
 */
$messages['bcc'] = array(
	'cite_desc' => 'اضفافه کنت<nowiki><ref[ name=id]></nowiki> و <nowiki><references/></nowiki> تگ, په ارجاع دهگ',
	'cite_croak' => 'ذکر منبع چه بن رپت; $1: $2',
	'cite_error_key_str_invalid' => 'حطا درونی ;
نامعتبرین $str و/یا  $key.
شی نباید هچ وهد پیش کیت',
	'cite_error_stack_invalid_input' => 'درونی حطا;
نامعتربین دسته کلیت.
شی نبایدن هچ وهد پیش کیت.',
	'cite_error' => 'حطا ارجاع: $1',
	'cite_error_ref_numeric_key' => 'نامعتبر <code>&lt;ref&gt;</code>تگ;
نام یک سادگین هوری نه نه بیت. یک توضیحی عنوانی استفاده کنیت',
	'cite_error_ref_no_key' => 'نامعتبر<code>&lt;ref&gt;</code>تگ;
مراجع بی محتوا بایدن نامی داشته بنت',
	'cite_error_ref_too_many_keys' => 'نامعتبر<code>&lt;ref&gt;</code>تگ;
نامعتبر نامان, په داب بازین',
	'cite_error_ref_no_input' => 'نامعتبر <code>&lt;ref&gt;</code> تگ;
مراجع بی نام بایدن محتوا داشته بنت',
	'cite_error_references_invalid_input' => 'نامعتبر <code>&lt;references&gt;</code>تگ;
هچ ورودی اجازت نهنت. استفاده کن  <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'نامعتبر <code>&lt;references&gt;</code>تگ;
هچ پارامتری مجاز نهنت.
استفاده کن چه <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters_group' => 'نامعتبر <code>&lt;references&gt;</code>تگ;
پارامتر "گروه" فقط مجازنت.
استفاده کن چه <code>&lt;references /&gt;</code>, یا <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => 'هلگ برجسپان لینک عقب رسمی.
گیشتر تعریف کن ته <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki> کوله',
	'cite_error_references_no_text' => 'نامعتبر<code>&lt;ref&gt;</code>تگ;
په نام ارجاع هچ متنی دهگ نه بیته <code>$1</code>',
	'cite_reference_link_prefix' => 'هل_مرج-',
	'cite_references_link_prefix' => 'ذکرـیادداشت-',
	'cite_references_link_many_format_backlink_labels' => 'ا ب پ ت ج چ خ د ر ز س ش غ ف ک ل م ن و ه ی',
	'cite_references_link_many_sep' => 'س',
	'cite_references_link_many_and' => 'و',
);

/** Bulgarian (Български)
 * @author Borislav
 * @author DCLXVI
 * @author Spiritia
 */
$messages['bg'] = array(
	'cite_desc' => 'Добавя етикетите <nowiki><ref[ name=id]></nowiki> и <nowiki><references/></nowiki>, подходящи за цитиране',
	'cite_croak' => 'Цитиращата система се срути; $1: $2',
	'cite_error_key_str_invalid' => 'Вътрешна грешка: невалиден параметър $str и/или $key.  Това не би трябвало да се случва никога.',
	'cite_error_stack_invalid_input' => "'''Вътрешна грешка:''' невалиден ключ на стека. Това не би трябвало да се случва никога.",
	'cite_error' => 'Грешка при цитиране: $1',
	'cite_error_ref_numeric_key' => "'''Грешка в етикет <code>&lt;ref&gt;</code>:''' името не може да бъде число, използва се описателно име",
	'cite_error_ref_no_key' => "'''Грешка в етикет <code>&lt;ref&gt;</code>:''' етикетите без съдържание трябва да имат име",
	'cite_error_ref_too_many_keys' => "'''Грешка в етикет <code>&lt;ref&gt;</code>:''' грешка в името, например повече от едно име на етикета",
	'cite_error_ref_no_input' => "'''Грешка в етикет <code>&lt;ref&gt;</code>:''' етикетите без име трябва да имат съдържание",
	'cite_error_references_invalid_input' => "'''Грешка в етикет <code>&lt;references&gt;</code>:''' не е разрешен вход, използва се така: 
<code>&lt;references /&gt;</code>",
	'cite_error_references_invalid_parameters' => "'''Грешка в етикет <code>&lt;references&gt;</code>:''' използва се без параметри, така: <code>&lt;references /&gt;</code>",
	'cite_error_references_invalid_parameters_group' => 'Невалиден етикет <code>&lt;references&gt;</code>;
позволен е само параметър "group".
Използвайте <code>&lt;references /&gt;</code> или <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_text' => "'''Грешка в етикет <code>&lt;ref&gt;</code>:''' не е подаден текст за бележките на име <code>$1</code>",
	'cite_error_included_ref' => 'Липсва затварящ етикет &lt;/ref&gt; след отварящия етикет &lt;ref&gt;',
);

/** Bengali (বাংলা)
 * @author Bellayet
 * @author Zaheen
 */
$messages['bn'] = array(
	'cite_desc' => 'উদ্ধৃতির জন্য <nowiki><ref[ name=id]></nowiki> এবং <nowiki><references/></nowiki> ট্যাগসমূহ যোগ করুন',
	'cite_croak' => 'উদ্ধৃতি ক্রোক করা হয়েছে; $1: $2',
	'cite_error_key_str_invalid' => 'আভ্যন্তরীন ত্রুটি; অবৈধ $str এবং/অথবা $key। এটা কখনই ঘটা উচিত নয়।',
	'cite_error_stack_invalid_input' => 'আভ্যন্তরীন ত্রুটি; অবৈধ স্ট্যাক কি। এটা কখনই ঘটা উচিত নয়।',
	'cite_error' => 'উদ্ধৃতি ত্রুটি: $1',
	'cite_error_ref_numeric_key' => 'অবৈধ <code>&lt;ref&gt;</code> ট্যাগ; নাম কোন সরল পূর্ণসংখ্যা হতে পারবেনা, একটি বিবরণমূলক শিরোনাম ব্যবহার করুন',
	'cite_error_ref_no_key' => 'অবৈধ <code>&lt;ref&gt;</code> ট্যাগ; বিষয়বস্তুহীন refসমূহের অবশ্যই নাম থাকতে হবে',
	'cite_error_ref_too_many_keys' => 'অবৈধ <code>&lt;ref&gt;</code> ট্যাগ; অবৈধ নাম (যেমন- সংখ্যাতিরিক্ত)',
	'cite_error_ref_no_input' => 'অবৈধ <code>&lt;ref&gt;</code> ট্যাগ; নামবিহীন refসমূহের অবশ্যই বিষয়বস্তু থাকতে হবে',
	'cite_error_references_invalid_input' => 'অবৈধ <code>&lt;ref&gt;</code> ট্যাগ; কোন ইনপুট অনুমোদিত নয়, <code>&lt;references /&gt;</code> ব্যবহার করুন',
	'cite_error_references_invalid_parameters' => 'অবৈধ <code>&lt;ref&gt;</code> ট্যাগ; কোন প্যারামিটার অনুমোদিত নয়, <code>&lt;references /&gt;</code> ব্যবহার করুন',
	'cite_error_references_no_backlink_label' => 'পছন্দমাফিক ব্যাকলিংক লেবেলের সংখ্যা ফুরিয়ে গেছে, <nowiki>[[MediaWiki:Cite_references_link_many_format_backlink_labels]]</nowiki> বার্তায় আরও সংজ্ঞায়িত করুন',
	'cite_error_references_no_text' => 'অবৈধ <code>&lt;ref&gt;</code> ট্যাগ; <code>$1</code> নামের refগুলির জন্য কোন টেক্সট প্রদান করা হয়নি',
);

/** Breton (Brezhoneg)
 * @author Fulup
 */
$messages['br'] = array(
	'cite_desc' => 'Ouzhpennañ a ra ar balizennoù <nowiki><ref[ name=id]></nowiki> ha <nowiki><references/></nowiki>, evit an arroudoù.',
	'cite_croak' => 'Arroud breinet ; $1 : $2',
	'cite_error_key_str_invalid' => 'Fazi diabarzh ;
$str ha/pe alc\'hwez$ direizh.
Ne zlefe ket c\'hoarvezout gwezh ebet.',
	'cite_error_stack_invalid_input' => "Fazi diabarzh ;
alc'hwez pil direizh.
Ne zlefe ket c'hoarvezout gwezh ebet.",
	'cite_error' => 'Fazi arroud : $1',
	'cite_error_ref_numeric_key' => "Fazi implijout ar valizenn <code>&lt;ref&gt;</code> ;
n'hall ket an anv bezañ un niver anterin. Grit gant un titl deskrivus",
	'cite_error_ref_no_key' => "Fazi implijout ar valizenn <code>&lt;ref&gt;</code> ;
ret eo d'an daveennoù goullo kaout un anv",
	'cite_error_ref_too_many_keys' => 'Fazi implijout ar valizenn <code>&lt;ref&gt;</code> ;
anv direizh, niver re uhel da skouer',
	'cite_error_ref_no_input' => "Fazi implijout ar valizenn <code>&lt;ref&gt;</code> ;
ret eo d'an daveennoù hep anv bezañ danvez enno",
	'cite_error_references_invalid_input' => "Fazi implijout ar valizenn <code>&lt;ref&gt;</code> ;
n'eo aotreet moned ebet. Grit gant ar valizenn <code>&lt;references /&gt;</code>",
	'cite_error_references_invalid_parameters' => "Fazi implijout ar valizenn <code>&lt;ref&gt;</code> ;
n'eo aotreet arventenn ebet.
Grit gant ar valizenn <code>&lt;references /&gt;</code>",
	'cite_error_references_invalid_parameters_group' => 'Fazi implijout ar valizenn <code>&lt;ref&gt;</code> ;
n\'eus nemet an arventenn "strollad" zo aotreet.
Grit gant ar valizenn <code>&lt;references /&gt;</code>, pe <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => "N'eus ket a dikedennoù personelaet mui.
Spisait un niver brasoc'h anezho er gemennadenn <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki>",
	'cite_error_references_no_text' => 'Balizenn <code>&lt;ref&gt;</code> direizh ;
ne oa bet lakaet tamm testenn ebet evit ar valizenn <code>$1</code>',
);

/** Bosnian (Bosanski)
 * @author CERminator
 */
$messages['bs'] = array(
	'cite_error_stack_invalid_input' => 'Unutrašnja greška;
nepoznat "stack" ključ.
Ovo se ne bi smjelo događati.',
	'cite_error' => 'Greška kod citiranja: $1',
);

/** Catalan (Català)
 * @author Jordi Roqué
 * @author SMP
 */
$messages['ca'] = array(
	'cite_desc' => 'Afegeix les etiquetes <nowiki><ref[ name=id]></nowiki> i <nowiki><references/></nowiki>, per a cites',
	'cite_croak' => 'Cita corrompuda; $1: $2',
	'cite_error_key_str_invalid' => 'Error intern;
els valors $str i/o $key no valen.
Aquesta situació no s\'hauria de donar mai.',
	'cite_error_stack_invalid_input' => "Error intern;
el valor d'emmagatzematge no és vàlid.
Aquesta situació no s'hauria de donar mai.",
	'cite_error' => 'Error de cita: $1',
	'cite_error_ref_numeric_key' => 'Etiqueta <code>&lt;ref&gt;</code> no vàlida;
el nom no pot ser un nombre. Empreu una paraula o un títol descriptiu',
	'cite_error_ref_no_key' => 'Etiqueta <code>&lt;ref&gt;</code> no vàlida;
les refs sense contingut han de tenir nom',
	'cite_error_ref_too_many_keys' => 'Etiqueta <code>&lt;ref&gt;</code> no vàlida;
empreu l\'estructura <code>&lt;ref name="Nom"&gt;</code>',
	'cite_error_ref_no_input' => 'Etiqueta <code>&lt;ref&gt;</code> no vàlida; 
les referències sense nom han de tenir contingut',
	'cite_error_references_invalid_input' => 'Etiqueta <code>&lt;references&gt;</code> no vàlida;
no es permet afegir text. Useu <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'Etiqueta <code>&lt;references&gt;</code> no vàlida; 
no es permeten paràmetres. 
Useu <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters_group' => 'Etiqueta <code>&lt;references&gt;</code> no vàlida;
únicament es permet el paràmetre "group".
Useu <code>&lt;references /&gt;</code>, o <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_text' => "Etiqueta <code>&lt;ref&gt;</code> no vàlida;
no s'ha proporcionat text per les refs amb l'etiqueta <code>$1</code>",
);

/** Czech (Česky)
 * @author Danny B.
 * @author Li-sung
 * @author Matěj Grabovský
 * @author Mormegil
 */
$messages['cs'] = array(
	'cite_desc' => 'Přidává značky <nowiki><ref[ name="id"]></nowiki> a&nbsp;<nowiki><references /></nowiki> na označení citací',
	'cite_croak' => 'Nefunkční citace; $1: $2',
	'cite_error_key_str_invalid' => 'Vnitřní chyba; neplatný $str',
	'cite_error_stack_invalid_input' => 'Vnitřní chyba; neplatný klíč zásobníku',
	'cite_error' => 'Chybná citace $1',
	'cite_error_ref_numeric_key' => 'Chyba v tagu <code>&lt;ref&gt;</code>; názvem nesmí být prosté číslo, použijte popisné označení',
	'cite_error_ref_no_key' => 'Chyba v tagu <code>&lt;ref&gt;</code>; prázdné citace musí obsahovat název',
	'cite_error_ref_too_many_keys' => 'Chyba v tagu <code>&lt;ref&gt;</code>; chybné názvy, např. je jich příliš mnoho',
	'cite_error_ref_no_input' => 'Chyba v tagu <code>&lt;ref&gt;</code>; citace bez názvu musí mít vlastní obsah',
	'cite_error_references_invalid_input' => 'Chyba v tagu <code>&lt;references&gt;</code>; zde není dovolen vstup, použijte <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'Chyba v tagu <code>&lt;references&gt;</code>;  zde není dovolen parametr, použijte <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters_group' => 'Neplatná značka <tt>&lt;references&gt;</tt>;
je povolen pouze parametr „group“.
Použijte <tt>&lt;references /&gt;</tt> nebo <tt>&lt;references group="..." /&gt;</tt>.',
	'cite_error_references_no_backlink_label' => 'Došla označení zpětných odkazů, přidejte jich několik do zprávy <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki>',
	'cite_error_references_no_text' => 'Chyba v tagu <code>&lt;ref&gt;</code>; citaci označené <code>$1</code> není určen žádný text',
	'cite_error_included_ref' => 'Chybí ukončovací &lt;/ref&gt; k&nbsp;tagu &lt;ref&gt;',
);

/** Church Slavic (Словѣ́ньскъ / ⰔⰎⰑⰂⰡⰐⰠⰔⰍⰟ)
 * @author ОйЛ
 */
$messages['cu'] = array(
	'cite_references_link_many_format_backlink_labels' => 'а б в г д є ж ꙃ ꙁ и і к л м н о п р с т ф х ѡ ц ч ш щ ъ ꙑ ь ѣ ю ꙗ ѥ ѧ ѫ ѩ ѭ ѯ ѱ ѳ ѵ ѷ',
);

/** Welsh (Cymraeg)
 * @author Lloffiwr
 */
$messages['cy'] = array(
	'cite_desc' => 'Yn ychwanegu tagiau <nowiki><ref[ name=id]></nowiki> a <nowiki><references/></nowiki>, ar gyfer cyfeiriadau',
	'cite_croak' => 'Cyfeirio at farwolaeth; $1: $2',
	'cite_error_key_str_invalid' => 'Gwall mewnol;
$str a/neu $key annilys.
Ni ddylai hyn fyth ddigwydd.',
	'cite_error_stack_invalid_input' => 'Gwall mewnol;
Allwedd pentwr annilys.
Ni ddylai hyn fyth ddigwydd.',
	'cite_error' => 'Gwall cyfeirio: $1',
	'cite_error_ref_numeric_key' => 'Tag <code>&lt;ref&gt;</code> annilys;
ni all enw fod yn rif yn unig. Defnyddiwch deitl disgrifiadol.',
	'cite_error_ref_no_key' => 'Tag <code>&lt;ref&gt;</code> annilys;
rhaid i dagiau ref sydd heb gynnwys iddynt gael enw',
	'cite_error_ref_too_many_keys' => 'Tag <code>&lt;ref&gt;</code> annilys;
enwau annilys; e.e. gormod ohonynt',
	'cite_error_ref_no_input' => 'Tag <code>&lt;ref&gt;</code> annilys;
rhaid i dagiau ref heb enw iddynt gynnwys rhywbeth',
	'cite_error_references_invalid_input' => 'Tag <code>&lt;references&gt;</code> annilys;
ni chaniateir mewnbwn. Defnyddiwch <code>&lt;references /&gt;</code>',
	'cite_error_references_no_text' => "Tag <code>&lt;ref&gt;</code> annilys;
ni osodwyd unrhyw destun ar gyfer y 'ref' <code>$1</code>",
	'cite_error_included_ref' => '&lt;/ref&gt; clo yn eisiau ar gyfer y tag &lt;ref&gt;',
);

/** Danish (Dansk)
 * @author Morten LJ
 */
$messages['da'] = array(
	'cite_croak' => 'Fodnoten døde: $1: $2',
	'cite_error_key_str_invalid' => 'Intern fejl: Ugyldig $str og/eller $key. Dette burde aldrig forekomme.',
	'cite_error_stack_invalid_input' => 'Intern fejl: Ugyldig staknøgle. Dette burde aldrig forekomme.',
	'cite_error' => 'Fodnotefejl: $1',
	'cite_error_ref_numeric_key' => 'Ugyldigt <code>&lt;ref&gt;</code>-tag; "name" kan ikke være et simpelt heltal, brug en beskrivende titel',
	'cite_error_ref_no_key' => 'Ugyldigt <code>&lt;ref&gt;</code>-tag: Et <code>&lt;ref&gt;</code>-tag uden indhold skal have et navn',
	'cite_error_ref_too_many_keys' => 'Ugyldigt <code>&lt;ref&gt;</code>-tag: Ugyldige navne, fx for mange',
	'cite_error_ref_no_input' => 'Ugyldigt <code>&lt;ref&gt;</code>-tag: Et <code>&lt;ref&gt;</code>-tag uden navn skal have indhold',
	'cite_error_references_invalid_input' => 'Ugyldigt <code>&lt;references&gt;</code>-tag: Indhold ikke tilladt, brug i stedet <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'Ugyldig <code>&lt;references&gt;</code>-tag: Parametre er ikke tilladt, brug i stedet <code>&lt;references /&gt;</code>',
	'cite_error_references_no_backlink_label' => 'For mange <code>&lt;ref&gt;</code>-tags har det samme "name", tillad flere i beskeden <nowiki>[[MediaWiki:Cite_references_link_many_format_backlink_labels]]</nowiki>',
	'cite_error_references_no_text' => 'Ugyldigt <code>&lt;ref&gt;</code>-tag: Der er ikke specificeret nogen fodnotetekst til navnet <code>$1</code>',
);

/** German (Deutsch)
 * @author Raimond Spekking
 */
$messages['de'] = array(
	'cite_desc' => 'Ergänzt für Quellennachweise die <tt><nowiki><ref[ name=id]></nowiki></tt> und <tt><nowiki><references /></nowiki></tt>-Tags',
	'cite_croak' => 'Fehler im Referenz-System. $1: $2',
	'cite_error_key_str_invalid' => 'Interner Fehler: ungültiger $str und/oder $key. Dies sollte eigentlich gar nicht passieren können.',
	'cite_error_stack_invalid_input' => 'Interner Fehler: ungültiger „name“-stack. Dies sollte eigentlich gar nicht passieren können.',
	'cite_error' => 'Referenz-Fehler: $1',
	'cite_error_ref_numeric_key' => 'Ungültige <tt>&lt;ref&gt;</tt>-Verwendung: „name“ darf kein reiner Zahlenwert sein, benutze einen beschreibenden Namen.',
	'cite_error_ref_no_key' => 'Ungültige <tt>&lt;ref&gt;</tt>-Verwendung: „ref“ ohne Inhalt muss einen Namen haben.',
	'cite_error_ref_too_many_keys' => 'Ungültige <tt>&lt;ref&gt;</tt>-Verwendung: „name“ ist ungültig oder zu lang.',
	'cite_error_ref_no_input' => 'Ungültige <tt>&lt;ref&gt;</tt>-Verwendung: „ref“ ohne Namen muss einen Inhalt haben.',
	'cite_error_references_invalid_input' => 'Ungültige <tt>&lt;references&gt;</tt>-Verwendung: Es ist kein zusätzlicher Text erlaubt, verwende ausschließlich <tt><nowiki><references /></nowiki></tt>.',
	'cite_error_references_invalid_parameters' => 'Ungültige <tt>&lt;references&gt;</tt>-Verwendung: Es sind keine zusätzlichen Parameter erlaubt, verwende ausschließlich <tt><nowiki><references /></nowiki></tt>.',
	'cite_error_references_invalid_parameters_group' => 'Ungültige <code>&lt;references&gt;</code>-Verwendung: Nur der Parameter „group“ ist erlaubt, verwende <tt>&lt;references /&gt;</tt> oder <tt>&lt;references group="…" /&gt;</tt>',
	'cite_error_references_no_backlink_label' => 'Eine Referenz der Form <tt>&lt;ref name="…"/&gt;</tt> wird öfter benutzt als Buchstaben vorhanden sind. Ein Administrator muss <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki> um weitere Buchstaben/Zeichen ergänzen.',
	'cite_error_references_no_text' => 'Ungültiger <tt>&lt;ref&gt;</tt>-Tag; es wurde kein Text für das Ref mit dem Namen <tt>$1</tt> angegeben.',
	'cite_error_included_ref' => 'Es fehlt ein schließendes &lt;/ref&gt;',
);

/** German (formal address) (Deutsch (Sie-Form))
 * @author Raimond Spekking
 */
$messages['de-formal'] = array(
	'cite_error_ref_numeric_key' => 'Ungültige <tt>&lt;ref&gt;</tt>-Verwendung: „name“ darf kein reiner Zahlenwert sein, benutzen Sie einen beschreibenden Namen.',
	'cite_error_references_invalid_input' => 'Ungültige <tt>&lt;references&gt;</tt>-Verwendung: Es ist kein zusätzlicher Text erlaubt, verwenden Sie ausschließlich <tt><nowiki><references /></nowiki></tt>.',
	'cite_error_references_invalid_parameters' => 'Ungültige <tt>&lt;references&gt;</tt>-Verwendung: Es sind keine zusätzlichen Parameter erlaubt, verwenden Sie ausschließlich <tt><nowiki><references /></nowiki></tt>.',
);

/** Lower Sorbian (Dolnoserbski)
 * @author Michawiki
 */
$messages['dsb'] = array(
	'cite_desc' => 'Pśidawa toflicce <nowiki><ref[ name=id]></nowiki> a <nowiki><references/></nowiki> za pódaśa zrědłow',
	'cite_croak' => 'Zmólka w referencnem systemje. $1: $2',
	'cite_error_key_str_invalid' => 'Interna zmólka: njpłaśiwy $str a/abo $key. To njaměło se staś.',
	'cite_error_stack_invalid_input' => 'Interna zmólka: njepłaśiwy stackowy kluc. To njaměło se staś.',
	'cite_error' => 'Referencna zmólka: $1',
	'cite_error_ref_numeric_key' => 'Njepłaśiwa toflicka <code>&lt;ref&gt;</code>;
mě njamóžo jadnora licba byś. Wužyj wugroniwy titel',
	'cite_error_ref_no_key' => 'Njepłaśiwa toflicka <code>&lt;ref&gt;</code>;
"ref" bźez wopśimjeśa musy mě měś',
	'cite_error_ref_too_many_keys' => 'Njepłaśiwa toflicka <code>&lt;ref&gt;</code>;
njepłaśiwe mjenja, na pś. pśewjele',
	'cite_error_ref_no_input' => 'Njepłaśiwa toflicka <code>&lt;ref&gt;</code>;
"ref" bźez mjenja musy wopśimjeśe měś',
	'cite_error_references_invalid_input' => 'Njepłaśiwa toflicka <code>&lt;references&gt;</code>;
žedne zapódaśe dowólone. Wužyj <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'Njepłaśiwa toflicka <code>&lt;references&gt;</code>;
žedne parametry dowólone.
Wužyj <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters_group' => 'Njepłaśiwa toflicka <code>&lt;references&gt;</code>;
jano parameter "group" jo dowólony,
Wužyj <code>&lt;references /&gt;</code> abo <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_text' => 'Njepłaśiwa toflicka <code>&lt;ref&gt;</code>;
za ref z mjenim <code>$1</code> njejo se tekst pódał',
	'cite_error_included_ref' => 'Kóńceca toflicka &lt;/ref&gt; felujo za toflicku &lt;ref&gt;',
);

/** Greek (Ελληνικά)
 * @author Consta
 */
$messages['el'] = array(
	'cite_error_references_no_text' => 'Δεν δίνετε κείμενο.',
);

/** Esperanto (Esperanto)
 * @author Yekrats
 */
$messages['eo'] = array(
	'cite_desc' => 'Aldonas etikedojn <nowiki><ref[ name=id]></nowiki> kaj <nowiki><references/></nowiki> por citaĵoj',
	'cite_croak' => 'Cito mortis; $1: $2',
	'cite_error_key_str_invalid' => 'Interna eraro;
nevalida $str kaj/aŭ $key.
Ĉi tio neniam okazos.',
	'cite_error_stack_invalid_input' => 'Interna eraro;
nevalida staka ŝlosilo.
Ĉi tio verŝajne neniam okazus.',
	'cite_error' => 'Citaĵa eraro: $1',
	'cite_error_ref_numeric_key' => 'Nevalida etikedo <code>&lt;ref&gt;</code>;
nomo ne povas esti simpla entjero. Uzu priskriban titolon.',
	'cite_error_ref_no_key' => "Nevalida etikedo <code>&lt;ref&gt;</code>;
''ref'' kun nenia enhava nomo devas havi nomon",
	'cite_error_ref_too_many_keys' => 'Nevalida etikedo <code>&lt;ref&gt;</code>;
nevalidaj nomoj (ekz-e: tro multaj)',
	'cite_error_ref_no_input' => 'Nevalida etikedo <code>&lt;ref&gt;</code>;
ref-etikedoj sen nomo devas havi enhavojn.',
	'cite_error_references_invalid_input' => 'Nevalida etikedo <code>&lt;references&gt;</code>;
neniu enigo estas permesita. Uzu etikedon <code>&lt;references /&gt;</code>.',
	'cite_error_references_invalid_parameters' => 'Nevalida etikedo <code>&lt;references&gt;</code>; neniuj parametroj estas permesitaj, uzu <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters_group' => 'Nevalida etikedon <code>&lt;references&gt;</code>;
parametro "group" nur estas permesita.
Uzu etikedon <code>&lt;references /&gt;</code>, aŭ <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => 'Neniom plu memfaritaj retroligaj etikedoj.
Difinu pliajn en la mesaĝo <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki>.',
	'cite_error_references_no_text' => 'Nevalida <code>&lt;ref&gt;</code> etikedo;
neniu teksto estis donita por ref-oj nomataj <code>$1</code>',
	'cite_error_included_ref' => 'Ferma &lt;/ref&gt; mankas por &lt;ref&gt; etikedon',
);

/** Spanish (Español)
 * @author Muro de Aguas
 * @author Remember the dot
 * @author Sanbec
 */
$messages['es'] = array(
	'cite_desc' => 'Añade las etiquietas <nowiki><ref[ name=id]> y <references /></nowiki> para utilizar notas al pie.',
	'cite_croak' => "La extensión ''Cite'' se murió; $1: $2",
	'cite_error_key_str_invalid' => 'Error interno;
$str y/o $key no válido.
Esto nunca debe occurir.',
	'cite_error_stack_invalid_input' => 'Error interno;
la clave de la pila no es válida.
Esto nunca debe ocurrir.',
	'cite_error' => 'Error en la cita: $1',
	'cite_error_ref_numeric_key' => 'El elemento <code>&lt;ref&gt;</code> no es válido;
el nombre no puede ser un número entero simple. Use un título descriptivo',
	'cite_error_ref_no_key' => 'El elemento <code>&lt;ref&gt;</code> no es válido;
las referencias sin contenido deben tener un nombre',
	'cite_error_ref_too_many_keys' => 'El elemento <code>&lt;ref&gt;</code> no es válido;
nombres de parámetros no válidos',
	'cite_error_ref_no_input' => 'Elemento <code>&lt;ref&gt;</code> no válido;
referencias sin nombre deben tener contenido',
	'cite_error_references_invalid_parameters' => 'El elemento <code>&lt;references&gt;</code> no es válido;
no admite parámetros.
Use <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters_group' => 'El elemento <code>&lt;references&gt;</code> no es válido;
sólo se permite el parámetro «group».
Use <code>&lt;references /&gt;</code>, o <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_text' => 'El elemento <code>&lt;ref&gt;</code> no es válido;
pues no hay una referencia con texto llamada <code>$1</code>',
);

/** Basque (Euskara) */
$messages['eu'] = array(
	'cite_error' => 'Aipamen errorea: $1',
);

/** Persian (فارسی)
 * @author Huji
 */
$messages['fa'] = array(
	'cite_desc' => 'برچسب‌های <nowiki><ref[ name=id]></nowiki> و <nowiki><references/></nowiki> را برای یادکرد اضافه می‌کند',
	'cite_croak' => 'یادکرد خراب شد؛ $1: $2',
	'cite_error_key_str_invalid' => 'خطای داخلی؛ $str و/یا $key غیر مجاز. این خطا نباید هرگز رخ دهد.',
	'cite_error_stack_invalid_input' => 'خطای داخلی؛ کلید پشته غیرمجاز.  این خطا نباید هرگز رخ دهد.',
	'cite_error' => 'خطای یادکرد: $1',
	'cite_error_ref_numeric_key' => 'برچسب <code><ref></code> غیرمجاز؛ نام نمی‌تواند یک عدد باشد. عنوان واضح‌تری را برگزینید',
	'cite_error_ref_no_key' => 'برچسب <code><ref></code> غیرمجاز؛ یادکردهای بدون محتوا باید نام داشته باشند',
	'cite_error_ref_too_many_keys' => 'برچسب <code><ref></code> غیرمجاز؛ نام‌های غیرمجاز یا بیش از اندازه',
	'cite_error_ref_no_input' => 'برچسب <code><ref></code> غیرمجاز؛ یادکردهای بدون نام باید محتوا داشته باشند',
	'cite_error_references_invalid_input' => 'برچسب <code><references></code> غیرمجاز؛ ورود متن مجاز نیست، از <code><references /></code> استفاده کنید',
	'cite_error_references_invalid_parameters' => 'برچسب <code><references></code> غیرمجاز؛ استفاده از پارامتر مجاز است. از <code><references /></code> استفاده کنید',
	'cite_error_references_invalid_parameters_group' => 'برچسب <code>&lt;references&gt;</code> غیر مجاز؛ تنها پارامتر «group» قابل استفاده است.
از <code>&lt;references /&gt;</code> یا <code>&lt;references group="..." /&gt;</code> استفاده کنید',
	'cite_error_references_no_backlink_label' => 'برچسب‌های پیوند به انتها رسید.
موارد جدیدی را در پیام <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki> تعریف کنید',
	'cite_error_references_no_text' => 'برچسب <code><ref></code> غیرمجاز؛ متنی برای یادکردهای با نام <code>$1</code> وارد نشده‌است',
	'cite_error_included_ref' => 'برچسب تمام کنندهٔ &lt;/ref&gt; بدون برچسب &lt;ref&gt;',
);

/** Finnish (Suomi)
 * @author Agony
 * @author Nike
 * @author Str4nd
 */
$messages['fi'] = array(
	'cite_desc' => 'Tarjoaa <nowiki><ref[ name=id]></nowiki>- ja <nowiki><references/></nowiki>-elementit viittauksien tekemiseen.',
	'cite_croak' => 'Virhe viittausjärjestelmässä: $1: $2',
	'cite_error_key_str_invalid' => 'Sisäinen virhe: kelpaamaton $str ja/tai $key.',
	'cite_error_stack_invalid_input' => 'Sisäinen virhe: kelpaamaton pinoavain.',
	'cite_error' => 'Viittausvirhe: $1',
	'cite_error_ref_numeric_key' => 'Kelpaamaton <code>&lt;ref&gt;</code>-elementti: nimi ei voi olla numero – käytä kuvaavampaa nimeä.',
	'cite_error_ref_no_key' => 'Kelpaamaton <code>&lt;ref&gt;</code>-elementti: sisällöttömille refeille pitää määrittää nimi.',
	'cite_error_ref_too_many_keys' => 'Kelpaamaton <code>&lt;ref&gt;</code>-elementti: virheelliset nimet, esim. liian monta',
	'cite_error_ref_no_input' => 'Kelpaamaton <code>&lt;ref&gt;</code>-elementti: viitteillä ilman nimiä täytyy olla sisältöä',
	'cite_error_references_invalid_input' => 'Kelpaamaton <code>&lt;references&gt;</code>-elementti: sisällön lisääminen ei ole sallittu. Käytä elementtiä <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'Kelpaamaton <code>&lt;references&gt;</code>-elementti: parametrit eivät ole sallittuja. Käytä muotoa <code>&lt;references /&gt;</code>.',
	'cite_error_references_invalid_parameters_group' => 'Kelpaamaton <code>&lt;references&gt;</code>-elementti: vain parametri ”group” on sallittu. Käytä tagia <code>&lt;references /&gt;</code> tai <code>&lt;references group="…" /&gt;</code>',
	'cite_error_references_no_text' => 'Virheellinen <code>&lt;ref&gt;</code>-tagi;
viitettä <code>$1</code> ei löytynyt',
);

/** French (Français)
 * @author Cedric31
 * @author Grondin
 * @author IAlex
 * @author Sherbrooke
 * @author Verdy p
 */
$messages['fr'] = array(
	'cite_desc' => 'Ajoute les balises <nowiki><ref[ name=id]></nowiki> et <nowiki><references/></nowiki>, pour les citations.',
	'cite_croak' => 'Citation corrompue ; $1 : $2',
	'cite_error_key_str_invalid' => 'Erreur interne ; $str ou $key incorrects.
Ceci ne devrait jamais se produire.',
	'cite_error_stack_invalid_input' => 'Erreur interne ;
clé de pile incorrecte.
Ceci ne devrait jamais se produire.',
	'cite_error' => 'Erreur de citation : $1',
	'cite_error_ref_numeric_key' => 'Balise <code>&lt;ref&gt;</code> incorrecte ;
Le nom ne peut être un entier simple. Utilisez un titre descriptif.',
	'cite_error_ref_no_key' => 'Balise <code>&lt;ref&gt;</code> incorrecte ;
les références sans contenu doivent avoir un nom.',
	'cite_error_ref_too_many_keys' => 'Balise <code>&lt;ref&gt;</code> incorrecte ;
Noms incorrects, par exemple ils sont trop nombreux.',
	'cite_error_ref_no_input' => 'Balise <code>&lt;ref&gt;</code> incorrecte ;
les références sans nom doivent avoir un contenu.',
	'cite_error_references_invalid_input' => 'Balise <code>&lt;references&gt;</code> incorrecte ;
aucune entrée n’est permise. Utilisez simplement <code>&lt;references /&gt;</code>.',
	'cite_error_references_invalid_parameters' => "Balise <code>&lt;references&gt;</code> incorrecte ;
aucun paramètre n'est permis. Utilisez simplement <code>&lt;references /&gt;</code>.",
	'cite_error_references_invalid_parameters_group' => 'Balise <code>&lt;references&gt;</code> incorrecte ;
seul le paramètre « group » est autorisé. Utilisez <code>&lt;references /&gt;</code>, ou bien <code>&lt;references group="..." /&gt;</code>.',
	'cite_error_references_no_backlink_label' => 'Épuisement des étiquettes personnalisées.
Définissez-en un plus grand nombre dans le message <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki>.',
	'cite_error_references_no_text' => "Balise <code>&lt;ref&gt;</code> incorrecte ;
aucun texte n'a été fourni pour les références nommées <code>$1</code>.",
	'cite_error_included_ref' => 'Clôture &lt;/ref&gt; omise pour la balise &lt;ref&gt;',
	'cite_references_link_many_format_backlink_labels' => 'a b c d e f g h i j k l m n o p q r s t u v w x y z aa ab ac ad ae af ag ah ai aj ak al am an ao ap aq ar as at au av aw ax ay az ba bb bc bd be bf bg bh bi bj bk bl bm bn bo bp bq br bs bt bu bv bw bx by bz ca cb cc cd ce cf cg ch ci cj ck cl cm cn co cp cq cr cs ct cu cv cw cx cy cz da db dc dd de df dg dh di dj dk dl dm dn do dp dq dr ds dt du dv dw dx dy dz ea eb ec ed ee ef eg eh ei ej ek el em en eo ep eq er es et eu ev ew ex ey ez fa fb fc fd fe ff fg fh fi fj fk fl fm fn fo fp fq fr fs ft fu fv fw fx fy fz ga gb gc gd ge gf gg gh gi gj gk gl gm gn go gp gq gr gs gt gu gv gw gx gy gz ha hb hc hd he hf hg hh hi hj hk hl hm hn ho hp hq hr hs ht hu hv hw hx hy hz ia ib ic id ie if ig ih ii ij ik il im in io ip iq ir is it iu iv iw ix iy iz ja jb jc jd je jf jg jh ji jj jk jl jm jn jo jp jq jr js jt ju jv jw jx jy jz ka kb kc kd ke kf kg kh ki kj kk kl km kn ko kp kq kr ks kt ku kv kw kx ky kz la lb lc ld le lf lg lh li lj lk ll lm ln lo lp lq lr ls lt lu lv lw lx ly lz ma mb mc md me mf mg mh mi mj mk ml mm mn mo mp mq mr ms mt mu mv mw mx my mz na nb nc nd ne nf ng nh ni nj nk nl nm nn no np nq nr ns nt nu nv nw nx ny nz oa ob oc od oe of og oh oi oj ok ol om on oo op oq or os ot ou ov ow ox oy oz pa pb pc pd pe pf pg ph pi pj pk pl pm pn po pp pq pr ps pt pu pv pw px py pz qa qb qc qd qe qf qg qh qi qj qk ql qm qn qo qp qq qr qs qt qu qv qw qx qy qz ra rb rc rd re rf rg rh ri rj rk rl rm rn ro rp rq rr rs rt ru rv rw rx ry rz sa sb sc sd se sf sg sh si sj sk sl sm sn so sp sq sr ss st su sv sw sx sy sz ta tb tc td te tf tg th ti tj tk tl tm tn to tp tq tr ts tt tu tv tw tx ty tz ua ub uc ud ue uf ug uh ui uj uk ul um un uo up uq ur us ut uu uv uw ux uy uz va vb vc vd ve vf vg vh vi vj vk vl vm vn vo vp vq vr vs vt vu vv vw vx vy vz wa wb wc wd we wf wg wh wi wj wk wl wm wn wo wp wq wr ws wt wu wv ww wx wy wz xa xb xc xd xe xf xg xh xi xj xk xl xm xn xo xp xq xr xs xt xu xv xw xx xy xz ya yb yc yd ye yf yg yh yi yj yk yl ym yn yo yp yq yr ys yt yu yv yw yx yy yz za zb zc zd ze zf zg zh zi zj zk zl zm zn zo zp zq zr zs zt zu zv zw zx zy zz',
);

/** Franco-Provençal (Arpetan)
 * @author ChrisPtDe
 */
$messages['frp'] = array(
	'cite_desc' => 'Apond les balises <nowiki><ref[ name=id]></nowiki> et <nowiki><references/></nowiki>, por les citacions.',
	'cite_croak' => 'Citacion corrompua ; $1 : $2',
	'cite_error_key_str_invalid' => 'Èrror de dedens ; $str atendua.',
	'cite_error_stack_invalid_input' => 'Èrror de dedens ; cllâf de pila envalida.',
	'cite_error' => 'Èrror de citacion $1',
	'cite_error_ref_numeric_key' => 'Apèl envalido ; cllâf pas entègrâla atendua.',
	'cite_error_ref_no_key' => 'Apèl envalido ; niona cllâf spècefiâ.',
	'cite_error_ref_too_many_keys' => 'Apèl envalido ; cllâfs envalides, per ègzemplo, trop de cllâfs spècefiâs ou ben cllâf fôssa.',
	'cite_error_ref_no_input' => 'Apèl envalido ; niona entrâ spècefiâ.',
	'cite_error_references_invalid_input' => 'Entrâ envalida ; entrâ atendua.',
	'cite_error_references_invalid_parameters' => 'Arguments envalidos ; argument atendu.',
	'cite_error_references_no_backlink_label' => 'Ègzécucion en defôr de les ètiquètes pèrsonalisâs, dèfenésséd de ples dens lo mèssâjo <nowiki>[[MediaWiki:Cite_references_link_many_format_backlink_labels]]</nowiki>',
	'cite_error_references_no_text' => 'Nion tèxte endicâ por les refèrences siuventes : <code>$1</code>',
);

/** Galician (Galego)
 * @author Alma
 * @author Toliño
 * @author Xosé
 */
$messages['gl'] = array(
	'cite_desc' => 'Engade <nowiki><ref[ nome=id]></nowiki> e etiquetas <nowiki><references/></nowiki>, para notas',
	'cite_croak' => 'Cita morta; $1: $2',
	'cite_error_key_str_invalid' => 'Erro interno; $str e/ou $key inválidos. Isto non debera ocorrer.',
	'cite_error_stack_invalid_input' => 'Erro interno; stack key inválido. Isto non debera ocorrer.',
	'cite_error' => 'Erro no código da cita: $1',
	'cite_error_ref_numeric_key' => 'etiqueta <code>&lt;ref&gt;</code> non válida;
o nome non pode ser un simple entero: use un título descritivo',
	'cite_error_ref_no_key' => 'etiqueta <code>&lt;ref&gt;</code> non válida;
as referencias que non teñan contido deben ter un nome',
	'cite_error_ref_too_many_keys' => 'etiqueta <code>&lt;ref&gt;</code> non válida;
nomes non válidos, por exemplo, demasiados',
	'cite_error_ref_no_input' => 'etiqueta <code>&lt;ref&gt;</code> non válida;
as referencias que non teñan nome, deben ter contido',
	'cite_error_references_invalid_input' => 'etiqueta <code>&lt;references&gt;</code> non válida;
non se permite esa entrada. Use <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'etiqueta <code>&lt;references&gt;</code> non válida;
non están permitidos eses parámetros.
Use <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters_group' => 'etiqueta <code>&lt;references&gt;</code> non válida;
só está permitido o parámetro "group" ("grupo").
Use <code>&lt;references /&gt;</code> ou <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => 'As etiquetas personalizadas esgotáronse.
Defina máis na mensaxe <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki>',
	'cite_error_references_no_text' => 'etiqueta <code>&lt;ref&gt;</code> non válida;
non se forneceu texto para as referencias de nome <code>$1</code>',
	'cite_error_included_ref' => 'peche a etiqueta &lt;/ref&gt; que lle falta á outra etiqueta &lt;ref&gt;',
	'cite_reference_link_key_with_num' => '$1_$2',
	'cite_reference_link_prefix' => 'cite_ref-',
	'cite_references_link_prefix' => 'cite_note-',
	'cite_references_link_many_format_backlink_labels' => 'a b c d e f g h i j k l m n o p q r s t u v w x y z aa ab ac ad ae af ag ah ai aj ak al am an ao ap aq ar as at au av aw ax ay az ba bb bc bd be bf bg bh bi bj bk bl bm bn bo bp bq br bs bt bu bv bw bx by bz ca cb cc cd ce cf cg ch ci cj ck cl cm cn co cp cq cr cs ct cu cv cw cx cy cz da db dc dd de df dg dh di dj dk dl dm dn do dp dq dr ds dt du dv dw dx dy dz ea eb ec ed ee ef eg eh ei ej ek el em en eo ep eq er es et eu ev ew ex ey ez fa fb fc fd fe ff fg fh fi fj fk fl fm fn fo fp fq fr fs ft fu fv fw fx fy fz ga gb gc gd ge gf gg gh gi gj gk gl gm gn go gp gq gr gs gt gu gv gw gx gy gz ha hb hc hd he hf hg hh hi hj hk hl hm hn ho hp hq hr hs ht hu hv hw hx hy hz ia ib ic id ie if ig ih ii ij ik il im in io ip iq ir is it iu iv iw ix iy iz ja jb jc jd je jf jg jh ji jj jk jl jm jn jo jp jq jr js jt ju jv jw jx jy jz ka kb kc kd ke kf kg kh ki kj kk kl km kn ko kp kq kr ks kt ku kv kw kx ky kz la lb lc ld le lf lg lh li lj lk ll lm ln lo lp lq lr ls lt lu lv lw lx ly lz ma mb mc md me mf mg mh mi mj mk ml mm mn mo mp mq mr ms mt mu mv mw mx my mz na nb nc nd ne nf ng nh ni nj nk nl nm nn no np nq nr ns nt nu nv nw nx ny nz oa ob oc od oe of og oh oi oj ok ol om on oo op oq or os ot ou ov ow ox oy oz pa pb pc pd pe pf pg ph pi pj pk pl pm pn po pp pq pr ps pt pu pv pw px py pz qa qb qc qd qe qf qg qh qi qj qk ql qm qn qo qp qq qr qs qt qu qv qw qx qy qz ra rb rc rd re rf rg rh ri rj rk rl rm rn ro rp rq rr rs rt ru rv rw rx ry rz sa sb sc sd se sf sg sh si sj sk sl sm sn so sp sq sr ss st su sv sw sx sy sz ta tb tc td te tf tg th ti tj tk tl tm tn to tp tq tr ts tt tu tv tw tx ty tz ua ub uc ud ue uf ug uh ui uj uk ul um un uo up uq ur us ut uu uv uw ux uy uz va vb vc vd ve vf vg vh vi vj vk vl vm vn vo vp vq vr vs vt vu vv vw vx vy vz wa wb wc wd we wf wg wh wi wj wk wl wm wn wo wp wq wr ws wt wu wv ww wx wy wz xa xb xc xd xe xf xg xh xi xj xk xl xm xn xo xp xq xr xs xt xu xv xw xx xy xz ya yb yc yd ye yf yg yh yi yj yk yl ym yn yo yp yq yr ys yt yu yv yw yx yy yz za zb zc zd ze zf zg zh zi zj zk zl zm zn zo zp zq zr zs zt zu zv zw zx zy zz',
);

/** Gujarati (ગુજરાતી)
 * @author Dsvyas
 */
$messages['gu'] = array(
	'cite_references_link_many_format_backlink_labels' => 'અ આ ઇ ઈ ઉ ઊ એ ઐ ઓ ઔ ક ખ ગ ઘ ચ છ જ ઝ ટ ઠ ડ ઢ ણ ત થ દ ધ ન પ ફ બ ભ મ ય ર લ વ શ ષ સ હ ળ ક્ષ જ્ઞ ઋ',
);

/** Hebrew (עברית)
 * @author Rotem Liss
 */
$messages['he'] = array(
	'cite_desc' => 'הוספת תגיות <nowiki><ref[ name=id]></nowiki> ו־<nowiki><references/></nowiki> עבור הערות שוליים',
	'cite_croak' => 'בהערה יש שגיאה; $1: $2',
	'cite_error_key_str_invalid' => 'שגיאה פנימית; $str ו/או $key שגויים. זהו באג בתוכנה.',
	'cite_error_stack_invalid_input' => 'שגיאה פנימית; מפתח שגוי במחסנית. זהו באג בתוכנה.',
	'cite_error' => 'שגיאת ציטוט: $1',
	'cite_error_ref_numeric_key' => 'תגית <code>&lt;ref&gt;</code> שגויה; שם לא יכול להיות מספר פשוט, יש להשתמש בכותרת תיאורית',
	'cite_error_ref_no_key' => 'תגית <code>&lt;ref&gt;</code> שגויה; להערות שוליים ללא תוכן חייב להיות שם',
	'cite_error_ref_too_many_keys' => 'תגית <code>&lt;ref&gt;</code> שגויה; שמות שגויים, למשל, רבים מדי',
	'cite_error_ref_no_input' => 'תגית <code>&lt;ref&gt;</code> שגויה; להערות שוליים ללא שם חייב להיות תוכן',
	'cite_error_references_invalid_input' => 'תגית <code>&lt;references&gt;</code> שגויה; לא ניתן לכתוב תוכן, יש להשתמש בקוד <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'תגית <code>&lt;references&gt;</code> שגויה; לא ניתן להשתמש בפרמטרים, יש להשתמש בקוד <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters_group' => 'תגית <code>&lt;references&gt;</code> שגויה;
רק הפרמטר "group" מותר לשימוש.
אנא השתמשו בקוד <code>&lt;references /&gt;</code>, או בקוד <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => 'נגמרו תוויות הקישורים המותאמים אישית, אנא הגדירו נוספים בהודעת המערכת <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki>',
	'cite_error_references_no_text' => 'תגית <code>&lt;ref&gt;</code> שגויה; לא נכתב טקסט עבור הערת השוליים בשם <code>$1</code>',
	'cite_error_included_ref' => 'חסרה תגית &lt;/ref&gt; המתאימה לתגית &lt;ref&gt;',
);

/** Hindi (हिन्दी)
 * @author Kaustubh
 */
$messages['hi'] = array(
	'cite_desc' => '<nowiki><ref[ name=id]></nowiki> और <nowiki><references/></nowiki> यह दो संदर्भ देनेके लिये इस्तेमालमें आने वाले शब्द बढाये जायेंगे।',
	'cite_croak' => 'संदर्भ दे नहीं पाये; $1: $2',
	'cite_error_key_str_invalid' => 'आंतर्गत गलती;
गलत $str या/और $key।
ऐसा होना नहीं चाहियें।',
	'cite_error_stack_invalid_input' => 'आंतर्गत गलती; गलत स्टॅक की। ऐसा होना नहीं चाहियें।',
	'cite_error' => 'गलती उद्घृत करें: $1',
	'cite_error_ref_numeric_key' => '<code>&lt;ref&gt;</code> गलत कोड; नाम यह पूर्णांकी संख्या नहीं हो सकता, कृपया माहितीपूर्ण शीर्षक दें',
	'cite_error_ref_no_key' => '<code>&lt;ref&gt;</code> गलत कोड; खाली संदर्भोंको नाम होना आवश्यक हैं',
	'cite_error_ref_too_many_keys' => '<code>&lt;ref&gt;</code> गलत कोड; गलत नाम, उदा. ढेर सारी',
	'cite_error_ref_no_input' => '<code>&lt;ref&gt;</code> गलत कोड; नाम ना होने वाले संदर्भोंमें ज़ानकारी देना आवश्यक हैं',
	'cite_error_references_invalid_input' => '<code>&lt;ref&gt;</code> गलत कोड; इनपुट नहीं कर सकतें। <code>&lt;references /&gt;</code> का इस्तेमाल करें',
	'cite_error_references_invalid_parameters' => '<code>&lt;references&gt;</code> चुकीचा कोड; पॅरॅमीटर्स नहीं दे सकते, <code>&lt;references /&gt;</code> का इस्तेमाल करें',
	'cite_error_references_invalid_parameters_group' => '<code>&lt;references&gt;</code> गलत कोड; सिर्फ पॅरॅमीटर का "ग्रुप" इस्तेमाल में लाया जा सकता हैं, <code>&lt;references /&gt;</code> या फिर <code>&lt;references group="..." /&gt;</code> का इस्तेमाल करें',
	'cite_error_references_no_backlink_label' => 'तैयार किये हुए पीछे की कड़ियां देनेवाले नाम खतम हुए हैं, अधिक नाम <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki> इस संदेश में बढायें',
	'cite_error_references_no_text' => '<code>&lt;ref&gt;</code> गलत कोड; <code>$1</code> नामके संदर्भमें ज़ानकारी नहीं हैं',
);

/** Croatian (Hrvatski)
 * @author Dalibor Bosits
 * @author Dnik
 * @author SpeedyGonsales
 */
$messages['hr'] = array(
	'cite_desc' => 'Dodaje <nowiki><ref[ name=id]></nowiki> i <nowiki><references/></nowiki> oznake, za citiranje',
	'cite_croak' => 'Nevaljan citat; $1: $2',
	'cite_error_key_str_invalid' => 'Unutrašnja greška: loš $str i/ili $key. Ovo se nikada ne bi smjelo dogoditi.',
	'cite_error_stack_invalid_input' => 'Unutrašnja greška; loš ključ stacka.  Ovo se nikada ne bi smjelo dogoditi.',
	'cite_error' => 'Greška u citiranju: $1',
	'cite_error_ref_numeric_key' => 'Loša <code>&lt;ref&gt;</code> oznaka; naziv ne smije biti jednostavni broj, koristite opisni naziv',
	'cite_error_ref_no_key' => 'Loša <code>&lt;ref&gt;</code> oznaka; ref-ovi bez sadržaja moraju imati naziv',
	'cite_error_ref_too_many_keys' => 'Loša <code>&lt;ref&gt;</code> oznaka; loš naziv, npr. previše naziva',
	'cite_error_ref_no_input' => 'Loša <code>&lt;ref&gt;</code> oznaka; ref-ovi bez imena moraju imati sadržaj',
	'cite_error_references_invalid_input' => 'Loša <code>&lt;references&gt;</code> oznaka; nije dozvoljen unos, koristite
<code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'Loša <code>&lt;references&gt;</code> oznaka; parametri nisu dozvoljeni, koristite <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters_group' => 'Neispravna <code>&lt;references&gt;</code> oznaka;
Dopuštena je samo opcija "group".
Koristite <code>&lt;references /&gt;</code>, ili <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => 'Potrošene sve posebne oznake za poveznice unatrag, definirajte više u poruci <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki>',
	'cite_error_references_no_text' => 'Nije zadan tekst za izvor <code>$1</code>',
	'cite_error_included_ref' => 'Nedostaje zatvarajući &lt;/ref&gt; za &lt;ref&gt; oznaku',
);

/** Upper Sorbian (Hornjoserbsce)
 * @author Michawiki
 */
$messages['hsb'] = array(
	'cite_desc' => 'Přidawa taflički <nowiki><ref[ name=id]></nowiki> a <nowiki><references /></nowiki> za žórłowe podaća',
	'cite_croak' => 'Zmylk w referencnym systemje; $1: $2',
	'cite_error_key_str_invalid' => 'Interny zmylk: njepłaćiwy $str a/abo $key. To njeměło ženje wustupić.',
	'cite_error_stack_invalid_input' => 'Interny zmylk; njepłaćiwy kluč staploweho składa. To njeměło ženje wustupić.',
	'cite_error' => 'Referencny zmylk: $1',
	'cite_error_ref_numeric_key' => 'Njepłaćiwe wužiwanje taflički <code>&lt;ref&gt;</code>; "name" njesmě jednora hódnota integer być, wužij wopisowace mjeno.',
	'cite_error_ref_no_key' => 'Njepłaćiwe wužiwanje taflički <code>&lt;ref&gt;</code>; "ref" bjez wobsaha dyrbi mjeno měć.',
	'cite_error_ref_too_many_keys' => 'Njepłaćiwe wužiwanje taflički <code>&lt;ref&gt;</code>; njepłaćiwe mjena, na př. předołho',
	'cite_error_ref_no_input' => 'Njepłaćiwe wužiwanje taflički <code>&lt;ref&gt;</code>; "ref" bjez mjena dyrbi wobsah měć',
	'cite_error_references_invalid_input' => 'Njepłaćiwe wužiwanje taflički <code>&lt;references&gt;</code>; žadyn zapodaty tekst dowoleny, wužij jenož
<code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'Njepłaćiwe wužiwanje taflički <code>&lt;references&gt;</code>; žane parametry dowolene, wužij jenož <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters_group' => 'Njepłaćiwa taflička <code>&lt;references&gt;</code>;
jenož parameter "group" je dowoleny.
Wužij <code>&lt;references /&gt;</code> abo <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => 'Zwučene etikety wróćowotkazow wućerpjene.
Definuj wjace w powěsći <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki>',
	'cite_error_references_no_text' => 'Njepłaćiwa referenca formy <code>&lt;ref&gt;</code>; žadyn tekst za referency z mjenom  <code>$1</code> podaty.',
	'cite_error_included_ref' => 'Kónčny &lt;/ref&gt; za tafličku &lt;ref&gt; faluje',
);

/** Haitian (Kreyòl ayisyen)
 * @author Masterches
 */
$messages['ht'] = array(
	'cite_desc' => 'Ajoute baliz sa yo <nowiki><ref[ name=id]></nowiki> epi <nowiki><referans/></nowiki>, pou sitasyon yo.',
	'cite_croak' => 'Sitasyon sa pa bon ; $1 : $2',
	'cite_error_key_str_invalid' => 'Erè nan sistèm an : $str te dwèt parèt.
Erè sa pat janm dwèt rive.',
	'cite_error_stack_invalid_input' => 'Erè nan sistèm an ; 
kle pil an pa bon ditou.
Sa pa te dwe janm rive',
	'cite_error' => 'Erè nan sitasyon : $1',
	'cite_error_ref_numeric_key' => 'Apèl ou fè an pa bon ; se kle ki pa entegral, ki pat long nou tap tann',
	'cite_error_ref_no_key' => 'Apèl sa pa bon : nou pa bay pyès kle',
	'cite_error_ref_too_many_keys' => 'Apèl ou fè an pa bon ; kle yo pa bon, pa egzanp, nou bay twòp kle oubyen kle yo pa bon oubyen nou pa byen rantre yo nan sistèm an.',
	'cite_error_ref_no_input' => 'Apèl ou fè an pa bon ; ou pa presize pyès antre',
	'cite_error_references_invalid_input' => 'Sa ou antre a pa bon <code>&lt;references&gt;</code>; antre ki te dwe vini <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'Agiman, paramèt sa yo pa bon ; agiman nou tap tann',
	'cite_error_references_invalid_parameters_group' => 'Kòd sa pa bon <code>&lt;referans&gt;</code> ;

sèl paramèt ki otorize se « group » (gwoup).

Itilize <code>&lt;references /&gt;</code>, oubyen <code>&lt;references group="..." /&gt;</code> pito.',
	'cite_error_references_no_backlink_label' => 'Pa genyen etikèt, labèl pèsonalize ankò, presize yon nonm, yon kantite ki ta pli gran pase sa ou te bay pli bonè nan mesaj ou an <nowiki>[[MediaWiki:Cite_references_link_many_format_backlink_labels]]</nowiki>',
	'cite_error_references_no_text' => 'Baliz <code>&lt;ref&gt;</code> sa pa bon;
Nou pa bay pyès tèks pou referans nou nonmen yo <code>$1</code>',
);

/** Hungarian (Magyar)
 * @author Dani
 * @author KossuthRad
 */
$messages['hu'] = array(
	'cite_desc' => 'Lehetővé teszi idézések létrehozását <nowiki><ref[ name=id]></nowiki> és <nowiki><references/></nowiki> tagek segítségével',
	'cite_croak' => 'Sikertelen forráshivatkozás; $1: $2',
	'cite_error_key_str_invalid' => 'Belső hiba; érvénytelen $str és/vagy $key. Ennek soha nem kellene előfordulnia.',
	'cite_error_stack_invalid_input' => 'Belső hiba; érvénytelen kulcs. Ennek soha nem kellene előfordulnia.',
	'cite_error' => 'Hiba a forráshivatkozásban: $1',
	'cite_error_ref_numeric_key' => 'Érvénytelen <code>&lt;ref&gt;</code> tag; a name értéke nem lehet csupán egy szám, használj leíró címeket',
	'cite_error_ref_no_key' => 'Érvénytelen <code>&lt;ref&gt;</code> tag; a tartalom nélküli ref-eknek kötelező nevet (name) adni',
	'cite_error_ref_too_many_keys' => 'Érvénytelen <code>&lt;ref&gt;</code> tag; hibás nevek, pl. túl sok',
	'cite_error_ref_no_input' => 'Érvénytelen <code>&lt;ref&gt;</code> tag; a név (name) nélküli ref-eknek adni kell valamilyen tartalmat',
	'cite_error_references_invalid_input' => 'Érvénytelen <code>&lt;references&gt;</code> tag; nem lehet neki tartalmat adni, használd a
<code>&lt;references /&gt;</code> formát',
	'cite_error_references_invalid_parameters' => 'Érvénytelen <code>&lt;references&gt;</code> tag; nincsenek paraméterei, használd a <code>&lt;references /&gt;</code> formát',
	'cite_error_references_invalid_parameters_group' => 'Érvénytelen <code>&lt;references&gt;</code> tag; csak a „group” attribútum használható. Használd a <code>&lt;references /&gt;</code>, vagy a <code>&lt;references group="..." /&gt;</code> formát.',
	'cite_error_references_no_backlink_label' => 'Elfogytak a visszahivatkozásra használt címkék, adj meg többet a <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki> üzenetben',
	'cite_error_references_no_text' => 'Érvénytelen <code>&lt;ref&gt;</code> tag; nincs megadva szöveg a(z) <code>$1</code> nevű ref-eknek',
);

/** Interlingua (Interlingua)
 * @author McDutchie
 */
$messages['ia'] = array(
	'cite_desc' => 'Adde etiquettas <nowiki><ref[ name=id]></nowiki> e <nowiki><references/></nowiki>, pro citationes',
	'cite_croak' => 'Citation corrumpite; $1: $2',
	'cite_error_key_str_invalid' => 'Error interne;
clave $str e/o $key invalide.
Isto non deberea jammais occurrer.',
	'cite_error_stack_invalid_input' => 'Error interne;
clave de pila invalide.
Isto non deberea jammais occurrer.',
	'cite_error' => 'Error de citation: $1',
	'cite_error_ref_numeric_key' => 'Etiquetta <code>&lt;ref&gt;</code> invalide;
le nomine non pote esser un numero integre. Usa un titulo descriptive',
	'cite_error_ref_no_key' => 'Etiquetta <code>&lt;ref&gt;</code> invalide;
le refs sin contento debe haber un nomine',
	'cite_error_ref_too_many_keys' => 'Etiquetta <code>&lt;ref&gt;</code> invalide;
nomines invalide, p.ex. troppo de nomines',
	'cite_error_ref_no_input' => 'Etiquetta <code>&lt;ref&gt;</code> invalide;
le refs sin nomine debe haber contento',
	'cite_error_references_invalid_input' => 'Etiquetta <code>&lt;references&gt;</code> invalide;
nulle entrata es permittite. Usa <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'Etiquetta <code>&lt;references&gt;</code> invalide;
nulle parametros es permittite.
Usa <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters_group' => 'Etiquetta <code>&lt;references&gt;</code> invalide;
solmente le parametro "group" es permittite.
Usa <code>&lt;references /&gt;</code>, o <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => 'Le etiquettas de retroligamine personalisate es exhaurite.
Defini plus in le message <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki>',
	'cite_error_references_no_text' => 'Etiquetta <code>&lt;ref&gt;</code> invalide;
nulle texto esseva fornite pro le refs nominate <code>$1</code>',
	'cite_error_included_ref' => 'Le clausura &lt;/ref&gt; manca pro le etiquetta &lt;ref&gt;',
);

/** Indonesian (Bahasa Indonesia)
 * @author IvanLanin
 * @author Rex
 */
$messages['id'] = array(
	'cite_desc' => 'Menambahkan tag <nowiki><ref[ name=id]></nowiki> dan <nowiki><references/></nowiki> untuk kutipan',
	'cite_croak' => 'Kegagalan pengutipan; $1: $2',
	'cite_error_key_str_invalid' => 'Kesalahan internal; $str tak sah',
	'cite_error_stack_invalid_input' => 'Kesalahan internal; kunci stack tak sah',
	'cite_error' => 'Kesalahan pengutipan $1',
	'cite_error_ref_numeric_key' => 'Kesalahan pemanggilan; diharapkan suatu kunci non-integer',
	'cite_error_ref_no_key' => 'Kesalahan pemanggilan; tidak ada kunci yang dispesifikasikan',
	'cite_error_ref_too_many_keys' => 'Kesalahan pemanggilan; kunci tak sah, contohnya karena terlalu banyak atau tidak ada kunci yang dispesifikasikan',
	'cite_error_ref_no_input' => 'Kesalahan pemanggilan; tidak ada masukan yang dispesifikasikan',
	'cite_error_references_invalid_input' => 'Kesalahan masukan; seharusnya tidak ada',
	'cite_error_references_invalid_parameters' => 'Paramater tak sah; seharusnya tidak ada',
	'cite_error_references_invalid_parameters_group' => 'Tag <code>&lt;references&gt;</code> tidak valid;
hanya parameter "group" yang diizinkan.
Gunakan tag <code>&lt;references /&gt;</code>, atau <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => 'Kehabisan label pralana balik tersuai.
Tambahkan lagi di pesan sistem <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki>',
	'cite_error_references_no_text' => 'Tag <code>&lt;ref&gt;</code> tak valid; tak ditemukan teks dengan ref dengan nama <code>$1</code>',
	'cite_error_included_ref' => 'Tag &lt;ref&gt; harus ditutup oleh &lt;/ref&gt;',
);

/** Italian (Italiano)
 * @author BrokenArrow
 * @author Darth Kule
 */
$messages['it'] = array(
	'cite_desc' => 'Aggiunge i tag <nowiki><ref[ name=id]></nowiki> e <nowiki><references/></nowiki> per gestire le citazioni',
	'cite_croak' => 'Errore nella citazione: $1: $2',
	'cite_error_key_str_invalid' => 'Errore interno: $str errato',
	'cite_error_stack_invalid_input' => 'Errore interno: chiave di stack errata',
	'cite_error' => 'Errore nella funzione Cite $1',
	'cite_error_ref_numeric_key' => "Errore nell'uso del marcatore <code>&lt;ref&gt;</code>: il nome non può essere un numero intero. Usare un titolo esteso",
	'cite_error_ref_no_key' => "Errore nell'uso del marcatore <code>&lt;ref&gt;</code>: i ref vuoti non possono essere privi di nome",
	'cite_error_ref_too_many_keys' => "Errore nell'uso del marcatore <code>&lt;ref&gt;</code>: nomi non validi (ad es. numero troppo elevato)",
	'cite_error_ref_no_input' => "Errore nell'uso del marcatore <code>&lt;ref&gt;</code>: i ref privi di nome non possono essere vuoti",
	'cite_error_references_invalid_input' => "Errore nell'uso del marcatoree <code>&lt;references&gt;</code>: input non ammesso, usare il marcatore
<code>&lt;references /&gt;</code>",
	'cite_error_references_invalid_parameters' => "Errore nell'uso del marcatore <code>&lt;references&gt;</code>: parametri non ammessi, usare il marcatore <code>&lt;references /&gt;</code>",
	'cite_error_references_invalid_parameters_group' => 'Errore nell\'uso del marcatore <code>&lt;references&gt;</code>;
solo il parametro "group" è permesso.
Usare <code>&lt;references /&gt;</code> oppure <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => 'Etichette di rimando personalizzate esaurite, aumentarne il numero nel messaggio <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki>',
	'cite_error_references_no_text' => 'Marcatore <code>&lt;ref&gt;</code> non valido; non è stato indicato alcun testo per il marcatore <code>$1</code>',
	'cite_error_included_ref' => '&lt;/ref&gt; di chiusura mancante per il marcatore &lt;ref&gt;',
);

/** Japanese (日本語)
 * @author Aotake
 * @author Ficell
 * @author JtFuruhata
 */
$messages['ja'] = array(
	'cite_desc' => '引用のためのタグ<nowiki><ref[ name=id]></nowiki> および <nowiki><references/></nowiki> を追加する',
	'cite_croak' => '引用タグ機能の重大なエラー; $1: $2',
	'cite_error_key_str_invalid' => '内部エラー; 無効な $str',
	'cite_error_stack_invalid_input' => '内部エラー; 無効なスタックキー',
	'cite_error' => '引用エラー $1',
	'cite_error_ref_numeric_key' => '無効な <code>&lt;ref&gt;</code> タグ: 名前に単純な数値は使用できません。',
	'cite_error_ref_no_key' => '無効な <code>&lt;ref&gt;</code> タグ: 引用句の内容がない場合には名前 （<code>name</code> 属性）が必要です',
	'cite_error_ref_too_many_keys' => '無効な <code>&lt;ref&gt;</code> タグ: 無効な名前（多すぎる、もしくは誤った指定）',
	'cite_error_ref_no_input' => '無効な <code>&lt;ref&gt;</code> タグ: 名前 （<code>name</code> 属性）がない場合には引用句の内容が必要です',
	'cite_error_references_invalid_input' => '無効な <code>&lt;references&gt;</code> タグ: 内容のあるタグは使用できません。 <code>&lt;references /&gt;</code> を用いてください。',
	'cite_error_references_invalid_parameters' => '無効な <code>&lt;references&gt;</code> タグ: 引数のあるタグは使用できません。 <code>&lt;references /&gt;</code> を用いてください。',
	'cite_error_references_invalid_parameters_group' => '無効な <code>&lt;references&gt;</code> タグです。使用できるパラメータは "group" のみです。 <code>&lt;references /&gt;</code> または <code>&lt;references group="..." /&gt;</code> を使用してください。',
	'cite_error_references_no_backlink_label' => 'バックリンクラベルが使用できる個数を超えました。<nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki> メッセージでの定義を増やしてください。',
	'cite_error_references_no_text' => '無効な <code>&lt;ref&gt;</code> タグ: <code>$1</code>という名前の引用句に対するテキストがありません。',
	'cite_error_included_ref' => '&lt;ref&gt; タグに対応する &lt;/ref&gt; が不足しています。',
	'cite_references_link_many_format_backlink_labels' => 'い ろ は に ほ へ と ち り ぬ る を わ か よ た れ そ つ ね な ら む う ゐ の お く や ま け ふ こ え て あ さ き ゆ め み し ゑ ひ も せ す ん イ ロ ハ ニ ホ ヘ ト チ リ ヌ ル ヲ ワ カ ヨ タ レ ソ ツ ネ ナ ラ ム ウ ヰ ノ オ ク ヤ マ ケ フ コ エ テ ア サ キ ユ メ ミ シ ヱ ヒ モ セ ス ン',
);

/** Jutish (Jysk)
 * @author Huslåke
 */
$messages['jut'] = array(
	'cite_croak' => 'Æ fodnåt døde; $1: $2',
	'cite_error_key_str_invalid' => 'Intern fejl: Ugyldeg $str og/æller $key. Dette burde aldreg førekåm.',
	'cite_error_stack_invalid_input' => 'Intern fejl: Ugyldeg staknøgle. Dette burde aldreg førekåm.',
	'cite_error' => 'Fodnåtfejl: $1',
	'cite_error_ref_numeric_key' => 'Ugyldigt <code>&lt;ref&gt;</code>-tag; "name" kan ikke være et simpelt heltal, brug en beskrivende titel',
	'cite_error_ref_no_key' => 'Ugyldigt <code>&lt;ref&gt;</code>-tag: Et <code>&lt;ref&gt;</code>-tag uden indhold skal have et navn',
	'cite_error_ref_too_many_keys' => 'Ugyldigt <code>&lt;ref&gt;</code>-tag: Ugyldege navne, fx før mange',
	'cite_error_ref_no_input' => 'Ugyldigt <code>&lt;ref&gt;</code>-tag: Et <code>&lt;ref&gt;</code>-tag uden navn skal have indhold',
	'cite_error_references_invalid_input' => 'Ugyldigt <code>&lt;references&gt;</code>-tag: Indhold ikke tilladt, brug i stedet <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'Ugyldig <code>&lt;references&gt;</code>-tag: Parametre er ikke tilladt, brug i stedet <code>&lt;references /&gt;</code>',
	'cite_error_references_no_backlink_label' => 'For mange <code>&lt;ref&gt;</code>-tags har det samme "name", tillad flere i beskeden <nowiki>[[MediaWiki:Cite_references_link_many_format_backlink_labels]]</nowiki>',
	'cite_error_references_no_text' => 'Ugyldigt <code>&lt;ref&gt;</code>-tag: Der er ikke specificeret nogen fodnotetekst til navnet <code>$1</code>',
);

/** Javanese (Basa Jawa)
 * @author Meursault2004
 * @author Pras
 */
$messages['jv'] = array(
	'cite_desc' => 'Nambahaké tag <nowiki><ref[ name=id]></nowiki> lan <nowiki><references/></nowiki> kanggo kutipan (sitat)',
	'cite_croak' => 'Sitaté (pangutipané) gagal; $1: $2',
	'cite_error_key_str_invalid' => 'Kaluputan jero;
$str lan/utawa $key ora absah.
Iki sajatiné ora tau olèh kadadéyan.',
	'cite_error_stack_invalid_input' => 'Kaluputan internal;
stack key ora absah.
Iki samesthine ora kadadéan.',
	'cite_error' => 'Kaluputan sitat (pangutipan) $1',
	'cite_error_ref_numeric_key' => 'Tag <code>&lt;ref&gt;</code> ora absah;
jenengé ora bisa namung angka integer waé. Gunakna irah-irahan (judhul) dèskriptif',
	'cite_error_ref_no_key' => 'Tag <code>&lt;ref&gt;</code> ora absah;
refs tanpa isi kudu duwé jeneng',
	'cite_error_ref_too_many_keys' => 'Tag <code>&lt;ref&gt;</code> ora absah;
jeneng-jenengé ora absah, contoné kakèhan',
	'cite_error_ref_no_input' => 'Tag <code>&lt;ref&gt;</code> ora absah;
refs tanpa jeneng kudu ana isiné',
	'cite_error_references_invalid_input' => 'Tag <code>&lt;references&gt;</code> ora absah;
input (panglebokan) ora diparengaké. Enggonen <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'Tag <code>&lt;references&gt;</code> ora absah;
ora ana paramèter sing diidinaké.
Gunakna <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters_group' => 'Tag <code>&lt;references&gt;</code> ora absah;
namung paramèter "group" sing diolèhaké.
Gunakna <code>&lt;references /&gt;</code>, utawa <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => 'Kentèkan label pranala balik.
Tambahna ing pesenan sistém <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki>',
	'cite_error_references_no_text' => 'Tag <code>&lt;ref&gt;</code> ora absah; 
ora ditemokaké tèks kanggo ref mawa jeneng <code>$1</code>',
	'cite_error_included_ref' => 'Tag &lt;ref&gt; kudu ditutup déning &lt;/ref&gt;',
);

/** Kazakh (Arabic script) (‫قازاقشا (تٴوتە)‬) */
$messages['kk-arab'] = array(
	'cite_croak' => 'دٵيەكسٶز الۋ سٵتسٸز بٸتتٸ; $1: $2',
	'cite_error_key_str_invalid' => 'ٸشكٸ قاتە; جارامسىز $str',
	'cite_error_stack_invalid_input' => 'ٸشكٸ قاتە; جارامسىز ستەك كٸلتٸ',
	'cite_error' => 'دٵيەكسٶز الۋ $1 قاتەسٸ',
	'cite_error_ref_numeric_key' => 'جارامسىز <code>&lt;ref&gt;</code> بەلگٸشەسٸ; اتاۋ كٵدٸمگٸ بٷتٸن سان بولۋى مٷمكٸن ەمەس, سيپپاتاۋىش اتاۋ قولدانىڭىز',
	'cite_error_ref_no_key' => 'جارامسىز <code>&lt;ref&gt;</code> بەلگٸشەسٸ; ماعلۇماتسىز تٷسٸنٸكتەمەلەردە اتاۋ بولۋى قاجەت',
	'cite_error_ref_too_many_keys' => 'جارامسىز <code>&lt;ref&gt;</code> بەلگٸشە; جارامسىز اتاۋلار, مىسالى, تىم كٶپ',
	'cite_error_ref_no_input' => 'جارامسىز <code>&lt;ref&gt;</code> بەلگٸشە; اتاۋسىز تٷسٸنٸكتەمەلەردە ماعلۇماتى بولۋى قاجەت',
	'cite_error_references_invalid_input' => 'جارامسىز <code>&lt;references&gt;</code> بەلگٸشە; ەش كٸرٸس رۇقسات ەتٸلمەيدٸ, بىلاي <code>&lt;references /&gt;</code> قولدانىڭىز',
	'cite_error_references_invalid_parameters' => 'جارامسىز <code>&lt;references&gt;</code> بەلگٸشە; ەش باپتار رۇقسات ەتٸلمەيدٸ, بىلاي <code>&lt;references /&gt;</code> قولدانىڭىز',
	'cite_error_references_no_backlink_label' => 'قوسىمشا بەلگٸلەردٸڭ سانى بٸتتٸ, ودان ٵرٸ كٶبٸرەك <nowiki>[[MediaWiki:Cite_references_link_many_format_backlink_labels]]</nowiki> جٷيە حابارىندا بەلگٸلەڭٸز',
);

/** Kazakh (Cyrillic) (Қазақша (Cyrillic)) */
$messages['kk-cyrl'] = array(
	'cite_croak' => 'Дәйексөз алу сәтсіз бітті; $1: $2',
	'cite_error_key_str_invalid' => 'Ішкі қате; жарамсыз $str',
	'cite_error_stack_invalid_input' => 'Ішкі қате; жарамсыз стек кілті',
	'cite_error' => 'Дәйексөз алу $1 қатесі',
	'cite_error_ref_numeric_key' => 'Жарамсыз <code>&lt;ref&gt;</code> белгішесі; атау кәдімгі бүтін сан болуы мүмкін емес, сиппатауыш атау қолданыңыз',
	'cite_error_ref_no_key' => 'Жарамсыз <code>&lt;ref&gt;</code> белгішесі; мағлұматсыз түсініктемелерде атау болуы қажет',
	'cite_error_ref_too_many_keys' => 'Жарамсыз <code>&lt;ref&gt;</code> белгіше; жарамсыз атаулар, мысалы, тым көп',
	'cite_error_ref_no_input' => 'Жарамсыз <code>&lt;ref&gt;</code> белгіше; атаусыз түсініктемелерде мағлұматы болуы қажет',
	'cite_error_references_invalid_input' => 'Жарамсыз <code>&lt;references&gt;</code> белгіше; еш кіріс рұқсат етілмейді, былай <code>&lt;references /&gt;</code> қолданыңыз',
	'cite_error_references_invalid_parameters' => 'Жарамсыз <code>&lt;references&gt;</code> белгіше; еш баптар рұқсат етілмейді, былай <code>&lt;references /&gt;</code> қолданыңыз',
	'cite_error_references_no_backlink_label' => 'Қосымша белгілердің саны бітті, одан әрі көбірек <nowiki>[[MediaWiki:Cite_references_link_many_format_backlink_labels]]</nowiki> жүйе хабарында белгілеңіз',
);

/** Kazakh (Latin) (Қазақша (Latin)) */
$messages['kk-latn'] = array(
	'cite_croak' => 'Däýeksöz alw sätsiz bitti; $1: $2',
	'cite_error_key_str_invalid' => 'İşki qate; jaramsız $str',
	'cite_error_stack_invalid_input' => 'İşki qate; jaramsız stek kilti',
	'cite_error' => 'Däýeksöz alw $1 qatesi',
	'cite_error_ref_numeric_key' => 'Jaramsız <code>&lt;ref&gt;</code> belgişesi; ataw kädimgi bütin san bolwı mümkin emes, sïppatawış ataw qoldanıñız',
	'cite_error_ref_no_key' => 'Jaramsız <code>&lt;ref&gt;</code> belgişesi; mağlumatsız tüsiniktemelerde ataw bolwı qajet',
	'cite_error_ref_too_many_keys' => 'Jaramsız <code>&lt;ref&gt;</code> belgişe; jaramsız atawlar, mısalı, tım köp',
	'cite_error_ref_no_input' => 'Jaramsız <code>&lt;ref&gt;</code> belgişe; atawsız tüsiniktemelerde mağlumatı bolwı qajet',
	'cite_error_references_invalid_input' => 'Jaramsız <code>&lt;references&gt;</code> belgişe; eş kiris ruqsat etilmeýdi, bılaý <code>&lt;references /&gt;</code> qoldanıñız',
	'cite_error_references_invalid_parameters' => 'Jaramsız <code>&lt;references&gt;</code> belgişe; eş baptar ruqsat etilmeýdi, bılaý <code>&lt;references /&gt;</code> qoldanıñız',
	'cite_error_references_no_backlink_label' => 'Qosımşa belgilerdiñ sanı bitti, odan äri köbirek <nowiki>[[MediaWiki:Cite_references_link_many_format_backlink_labels]]</nowiki> jüýe xabarında belgileñiz',
);

/** Korean (한국어)
 * @author Ficell
 * @author Kwj2772
 * @author ToePeu
 */
$messages['ko'] = array(
	'cite_desc' => '인용에 쓰이는 <nowiki><ref[ name=id]></nowiki>와 <nowiki><references/></nowiki>태그를 더합니다.',
	'cite_croak' => '인용 오류; $1: $2',
	'cite_error' => '인용 오류: $1',
	'cite_error_ref_no_key' => '<code>&lt;ref&gt;</code> 태그가 잘못되었습니다;
내용이 없는 주석은 이름이 있어야 합니다.',
	'cite_error_references_invalid_parameters' => '<code>&lt;references&gt;</code> 태그가 잘못되었습니다;
변수를 넣어서는 안 됩니다.
<code>&lt;references /&gt;</code>를 이용하십시오.',
	'cite_references_link_many_format_backlink_labels' => '가 나 다 라 마 바 사 아 자 차 카 타 파 하 거 너 더 러 머 버 서 어 저 처 커 터 퍼 허 고 노 도 로 모 보 소 오 조 초 코 토 포 호 구 누 두 루 무 부 수 우 주 추 쿠 투 푸 후 그 느 드 르 므 브 스 으 즈 츠 크 트 프 흐 기 니 디 리 미 비 시 이 지 치 키 티 피 히',
);

/** Ripoarisch (Ripoarisch)
 * @author Purodha
 */
$messages['ksh'] = array(
	'cite_desc' => 'Erlaub Quelle un Referenze met <tt><nowiki><ref[ name="id"]></nowiki></tt> un <tt><nowiki><references /></nowiki></tt> aanzejevve.',
	'cite_croak' => 'Fääler met Refenenze. $1: $2',
	'cite_error_key_str_invalid' => 'Interne Fähler in <i lang="en">cite</i>:
<code>$str</code> udder <code>$key</code> stemme nit.
Dat sull nie optredde.',
	'cite_error_stack_invalid_input' => 'Interne Fähler in <i lang="en">cite</i>:
Der <i lang="en">stack</i>-Schößel stemmp nit.
Dat sull nie optredde.',
	'cite_error' => 'Fähler in <i lang="en">cite</i> met Refenenze: $1',
	'cite_error_ref_numeric_key' => 'Fähler en <i lang="en">cite</i>:
Ene <code>&lt;ref&gt;</code>-Name kann kei Zahl sin.
Nemm enne Tittel, dä jät säht.',
	'cite_error_ref_no_key' => 'Fähler en <i lang="en">cite</i>:
E <code>&lt;ref&gt;</code> oohne Enhalt moß ene Name han.
Nemm enne Tittel, dä jät säht.',
	'cite_error_ref_too_many_keys' => 'Fähler en <i lang="en">cite</i>:
Zo fill <code>&lt;ref&gt;</code>-Name,
udder kapodde ene Name.',
	'cite_error_ref_no_input' => 'Fähler en <i lang="en">cite</i>:
E <code>&lt;ref&gt;</code> oohne Name moß ene Enhallt han.',
	'cite_error_references_invalid_input' => 'Fähler en <i lang="en">cite</i>:
E <code>&lt;references&gt;</code> moß oohne Enhalt sin.
Nemm eifach <code>&lt;references /&gt;</code> un söns nix.',
	'cite_error_references_invalid_parameters' => 'Fähler en <i lang="en">cite</i>:
E <code>&lt;references&gt;</code> moß oohne Parrametere sin.
Nemm eifach <code>&lt;references /&gt;</code> un söns nix.',
	'cite_error_references_invalid_parameters_group' => 'Fähler en <i lang="en">cite</i>:
E <code>&lt;references&gt;</code> darf nur dä Parrameeter „<code>group</code>“ han.
Nemm eifach <code>&lt;references /&gt;</code> udder <code>&lt;references group="..." /&gt;</code> un söns nix.',
	'cite_error_references_no_backlink_label' => 'Fähler en <i lang="en">cite</i>:
Nit jenoch Name för retuur-Lengks.
Donn mieh en dä Sigg <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki> enndrare.',
	'cite_error_references_no_text' => 'Fähler en <i lang="en">cite</i>:
Et wohr keine Tex aanjejovve för de
<code>&lt;ref&gt;</code>s met dämm Name „<code>$1</code>“.',
	'cite_error_included_ref' => 'Hee för dat &lt;ref&gt; ham_mer kei zopaß &lt;/ref&gt;',
	'cite_reference_link_key_with_num' => '$1_$2',
	'cite_reference_link_prefix' => 'cite_ref-',
	'cite_references_link_prefix' => 'cite_note-',
	'cite_reference_link' => '<sup id="$1" class="reference">[[#$2|<nowiki>[</nowiki>$3<nowiki>]</nowiki>]]</sup>',
	'cite_references_link_one' => '<li id="$1">[[#$2|↑]] $3</li>',
	'cite_references_link_many' => '<li id="$1">↑ $2 $3</li>',
	'cite_references_link_many_format' => '<sup>[[#$1|$2]]</sup>',
	'cite_references_link_many_format_backlink_labels' => 'a b c d e f g h i j k l m n o p q r s t u v w x y z',
);

/** Luxembourgish (Lëtzebuergesch)
 * @author Robby
 */
$messages['lb'] = array(
	'cite_desc' => 'Setzt <nowiki><ref[ name=id]></nowiki> an <nowiki><references/></nowiki> Taggen derbäi, fir Zitatiounen.',
	'cite_croak' => 'Feeler am Referenz-System. $1 : $2',
	'cite_error_key_str_invalid' => 'Interne Feeler;
ongültege $str an/oder $schlëssel.
Dëst sollt eigentlech ni gschéien.',
	'cite_error_stack_invalid_input' => "Interne Feeler;
ongëltege ''stack''-Schlëssel.
Dës sollt eigentlech guer net geschéien.",
	'cite_error' => 'Zitéierfeeler: $1',
	'cite_error_ref_numeric_key' => 'Ongëltegen <code>&lt;ref&gt;</code> Tag;
De Numm ka keng einfach ganz Zuel sinn. Benotzt w.e.g. een Titel den eng Beschreiwung gëtt',
	'cite_error_ref_no_key' => 'Ongëltegen <code>&lt;ref&gt;</code> Tag;
Referenzen ouni Inhalt mussen e Numm hunn',
	'cite_error_ref_too_many_keys' => 'Ongëltege <code>&lt;ref&gt;</code> Tag;
ongëlteg Nimm, z. Bsp. zevill',
	'cite_error_ref_no_input' => "Ongëltege <code>&lt;ref&gt;</code> Tag;
''refs'' ouni Numm muss een Inhalt hun",
	'cite_error_references_invalid_input' => 'Ongëltegen <code>&lt;references&gt;</code> Tag;
Keen Input ass erlaabt. Benotzt <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'Ongëltegen <code>&lt;references&gt;</code> Tag;
et si keng Parameter erlaabt.
Benotzt <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters_group' => 'Ongëltege  <code>&lt;references&gt;</code> Tag;
nëmmen de Parameter "group" ass erlaabt.
Benotzt <code>&lt;references /&gt;</code>, oder <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_text' => "Ongëltegen <code>&lt;ref&gt;</code> Tag;
et gouf keen Text uginn fir d'Referenzen mam Numm <code>$1</code>",
	'cite_error_included_ref' => 'Den Tag &lt;/ref&gt; feelt fir den Tag &lt;ref&gt; zouzemaachen',
);

/** Limburgish (Limburgs)
 * @author Ooswesthoesbes
 */
$messages['li'] = array(
	'cite_desc' => 'Voeg <nowiki><ref[ name=id]></nowiki> en <nowiki><references/></nowiki> tags toe veur citate',
	'cite_croak' => 'Perbleem mit Citere; $1: $2',
	'cite_error_key_str_invalid' => 'Interne fout; ónzjuuste $str en/of $key.  Dit zów noeaits mótte veurkómme.',
	'cite_error_stack_invalid_input' => 'Interne fout; ónzjuuste stacksleutel.  Dit zów noeaits mótte veurkómme.',
	'cite_error' => 'Citeerfout: $1',
	'cite_error_ref_numeric_key' => "Ónzjuuste tag <code>&lt;ref&gt;</code>; de naam kin gein simpele integer zeen, gebroek 'ne besjrievendje titel",
	'cite_error_ref_no_key' => "Ónzjuuste tag <code>&lt;ref&gt;</code>; refs zónger inhoud mótte 'ne naam höbbe",
	'cite_error_ref_too_many_keys' => 'Ónzjuuste tag <code>&lt;ref&gt;</code>; ónzjuuste name, beveurbeildj te väöl',
	'cite_error_ref_no_input' => 'Ónzjuuste tag <code>&lt;ref&gt;</code>; refs zónger naam mótte inhoud höbbe',
	'cite_error_references_invalid_input' => 'Ónzjuuste tag <code>&lt;references&gt;</code>; inveur is neet toegestaon, gebroek <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'Ónzjuuste tag <code>&lt;references&gt;</code>; paramaeters zeen neet toegestaon, gebroek <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters_group' => 'Onjuuste tag <code>&lt;references&gt;</code>;
allein de paramaeter "group" is toegestaon.
Gebruik <code>&lt;references /&gt;</code>, of <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => "'t Aantal besjikbare backlinklabels is opgebroek. Gaef meer labels op in 't berich <nowiki>[[MediaWiki:Cite_references_link_many_format_backlink_labels]]</nowiki>",
	'cite_error_references_no_text' => "Ónzjuuste tag <code>&lt;ref&gt;</code>; d'r is gein teks opgegaeve veur refs mit de naam <code>$1</code>",
);

/** Lithuanian (Lietuvių) */
$messages['lt'] = array(
	'cite_croak' => 'Cituoti nepavyko; $1: $2',
	'cite_error_key_str_invalid' => 'Vidinė klaida; neleistinas $str',
	'cite_error_stack_invalid_input' => 'Vidinė klaida; neleistinas steko raktas',
	'cite_error' => 'Citavimo klaida $1',
	'cite_error_ref_numeric_key' => 'Neleistina <code>&lt;ref&gt;</code> gairė; vardas negali būti tiesiog skaičius, naudokite tekstinį pavadinimą',
	'cite_error_ref_no_key' => 'Neleistina <code>&lt;ref&gt;</code> gairė; nuorodos be turinio turi turėti vardą',
	'cite_error_ref_too_many_keys' => 'Neleistina <code>&lt;ref&gt;</code> gairė; neleistini vardai, pvz., per daug',
	'cite_error_ref_no_input' => 'Neleistina <code>&lt;ref&gt;</code> gairė; nuorodos be vardo turi turėti turinį',
	'cite_error_references_invalid_input' => 'Neleistina <code>&lt;references&gt;</code> gairė; neleistina jokia įvestis, naudokite <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'Neleistina <code>&lt;references&gt;</code> gairė; neleidžiami jokie parametrai, naudokite <code>&lt;references /&gt;</code>',
	'cite_error_references_no_backlink_label' => 'Baigėsi antraštės, nurodykite daugiau <nowiki>[[MediaWiki:Cite_references_link_many_format_backlink_labels]]</nowiki> sisteminiame tekste',
);

/** Latvian (Latviešu)
 * @author Xil
 */
$messages['lv'] = array(
	'cite_desc' => 'Pievieno <nowiki><ref[ name=id]></nowiki> un <nowiki><references/></nowiki> tagus, atsaucēm',
	'cite_error' => 'Kļūda atsaucē: $1',
);

/** Malayalam (മലയാളം)
 * @author Shijualex
 */
$messages['ml'] = array(
	'cite_desc' => 'അവലംബം ചേര്‍ക്കുവാന്‍ ഉപയോഗിക്കാനുള്ള <nowiki><ref[ name=id]></nowiki>, <nowiki><references/></nowiki> എന്നീ ടാഗുകള്‍ ചേര്‍ക്കുന്നു',
	'cite_croak' => 'സൈറ്റ് ചത്തിരിക്കുന്നു; $1: $2',
	'cite_error_key_str_invalid' => 'ആന്തരിക പിഴവ്; 
അസാധുവായ $str അല്ലെങ്കില്‍ $key.
ഇതു ഒരിക്കലും സംഭവിക്കാന്‍ പാടില്ലായിരുന്നു.',
	'cite_error_stack_invalid_input' => 'ആന്തരിക പിഴവ്; അസാധുവായ സ്റ്റാക് കീ. ഇതു ഒരിക്കലും സംഭവിക്കാന്‍ പാടില്ലായിരുന്നു.',
	'cite_error' => 'ഉദ്ധരിച്ചതില്‍ പിഴവ്: $1',
	'cite_error_ref_numeric_key' => 'അസാധുവായ <code>&lt;ref&gt;</code> ടാഗ്;
നാമത്തില്‍ സംഖ്യ മാത്രമായി അനുവദനീയമല്ല. എന്തെങ്കിലും ലഘുവിവരണം ഉപയോഗിക്കുക.',
	'cite_error_ref_no_key' => 'അസാധുവായ <code>&lt;ref&gt;</code> ടാഗ്;
ഉള്ളടക്കമൊന്നുമില്ലാത്ത അവലംബത്തിനും ഒരു പേരു വേണം.',
	'cite_error_ref_too_many_keys' => 'അസാധുവായ <code>&lt;ref&gt;</code> ടാഗ്;
അസാധുവായ പേരുകള്‍, ഉദാ: too many',
	'cite_error_ref_no_input' => 'അസാധുവായ <code>&lt;ref&gt;</code> ടാഗ്;
പേരില്ലാത്ത അവലംബത്തിനു ഉള്ളടക്കമുണ്ടായിരിക്കണം.',
	'cite_error_references_invalid_input' => 'അസാധുവായ <code>&lt;references&gt;</code> ടാഗ്;
റെഫറന്‍സ് ടാഗിനകത്ത് ടെക്സ്റ്റ് അനുവദനീയമല്ല. പകരം ഇങ്ങനെ <code>&lt;references /&gt;</code> ചെയ്യാവുന്നതാണു.',
	'cite_error_references_invalid_parameters' => 'അസാധുവായ <code>&lt;references&gt;</code> ടാഗ്;
റെഫറന്‍സ് ടാഗിനകത്ത് പരാമീററ്ററുകള്‍ അനുവദനീയമല്ല. പകരം ഇങ്ങനെ <code>&lt;references /&gt;</code> ചെയ്യാവുന്നതാണു.',
	'cite_error_references_invalid_parameters_group' => 'അസാധുവായ <code>&lt;references&gt;</code> ടാഗ്;
റെഫറന്‍സ് ടാഗിനകത്ത് "group" പരാമീറ്റര്‍ മാത്രമേ അനുവദനീമായുള്ളൂ. പകരം ഇങ്ങനെ <code>&lt;references /&gt;</code>, അല്ലെങ്കില്‍ <code>&lt;references group="..." /&gt;</code> ചെയ്യാവുന്നതാണു.',
	'cite_error_references_no_text' => 'അസാധുവായ <code>&lt;ref&gt;</code> ടാഗ്;
<code>$1</code> എന്ന അവലംബങ്ങള്‍ക്ക് ടെക്സ്റ്റ് ഒന്നും കൊടുത്തിട്ടില്ല.',
);

/** Marathi (मराठी)
 * @author Kaustubh
 */
$messages['mr'] = array(
	'cite_desc' => '<nowiki><ref[ name=id]></nowiki> व <nowiki><references/></nowiki> हे दोन संदर्भ देण्यासाठी वापरण्यात येणारे शब्द वाढविले जातील.',
	'cite_croak' => 'संदर्भ देता आला नाही; $1: $2',
	'cite_error_key_str_invalid' => 'अंतर्गत त्रुटी; चुकीचे $str आणि/किंवा $key. असे कधीही घडले नाही पाहिजे.',
	'cite_error_stack_invalid_input' => 'अंतर्गत त्रुटी; चुकीची स्टॅक चावी. असे कधीही घडले नाही पाहिजे.',
	'cite_error' => 'त्रूटी उधृत करा: $1',
	'cite_error_ref_numeric_key' => '<code>&lt;ref&gt;</code> चुकीचा कोड; नाव हे पूर्णांकी संख्या असू शकत नाही, कृपया माहितीपूर्ण शीर्षक द्या',
	'cite_error_ref_no_key' => '<code>&lt;ref&gt;</code> चुकीचा कोड; रिकाम्या संदर्भांना नाव असणे गरजेचे आहे',
	'cite_error_ref_too_many_keys' => '<code>&lt;ref&gt;</code> चुकीचा कोड; चुकीची नावे, उदा. खूप सारी',
	'cite_error_ref_no_input' => '<code>&lt;ref&gt;</code> चुकीचा कोड; निनावी संदर्भांमध्ये माहिती असणे गरजेचे आहे',
	'cite_error_references_invalid_input' => '<code>&lt;references&gt;</code> चुकीचा कोड; माहिती देता येत नाही, <code>&lt;references /&gt;</code> हा कोड वापरा',
	'cite_error_references_invalid_parameters' => '<code>&lt;references&gt;</code> चुकीचा कोड; पॅरॅमीटर्स देता येत नाही, <code>&lt;references /&gt;</code> हा कोड वापरा',
	'cite_error_references_invalid_parameters_group' => 'चुकीची <code>&lt;references&gt;</code> खूण; फक्त पॅरॅमीटर चा गट वापरता येईल, <code>&lt;references /&gt;</code> किंवा <code>&lt;references group="..." /&gt;</code> चा वापर करा',
	'cite_error_references_no_backlink_label' => 'तयार केलेली मागीलदुवे देणारी नावे संपलेली आहेत, अधिक नावे <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki> या प्रणाली संदेशात लिहा',
	'cite_error_references_no_text' => '<code>&lt;ref&gt;</code> चुकीचा कोड; <code>$1</code> नावाने दिलेल्या संदर्भांमध्ये काहीही माहिती नाही',
);

/** Malay (Bahasa Melayu)
 * @author Aviator
 */
$messages['ms'] = array(
	'cite_desc' => 'Menambah tag <nowiki><ref[ name=id]></nowiki> dan <nowiki><references/></nowiki> untuk pemetikan',
	'cite_croak' => 'Ralat maut petik; $1: $2',
	'cite_error_key_str_invalid' => 'Ralat dalaman; str dan/atau $key tidak sah.',
	'cite_error_stack_invalid_input' => 'Ralat dalaman; kunci tindanan tidak sah.',
	'cite_error' => 'Ralat petik: $1',
	'cite_error_ref_numeric_key' => 'Tag <code>&lt;ref&gt;</code> tidak sah; nombor ringkas tidak dibenarkan, sila masukkan tajuk yang lebih terperinci',
	'cite_error_ref_no_key' => 'Tag <code>&lt;ref&gt;</code> tidak sah; rujukan tanpa kandungan mestilah mempunyai nama',
	'cite_error_ref_too_many_keys' => 'Tag <code>&lt;ref&gt;</code> tidak sah; nama-nama tidak sah, misalnya terlalu banyak',
	'cite_error_ref_no_input' => "'Tag <code>&lt;ref&gt;</code> tidak sah; rujukan tanpa nama mestilah mempunyai kandungan",
	'cite_error_references_invalid_input' => 'Tag <code>&lt;references&gt;</code> tidak sah; input tidak dibenarkan, gunakan <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'Tag <code>&lt;references&gt;</code> tidak sah; parameter tidak dibenarkan, gunakan <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters_group' => 'Tag <code>&lt;references&gt;</code> tidak sah; hanya parameter "group" dibenarkan.
Gunakan <code>&lt;references /&gt;</code> atau <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => 'Kehabisan label pautan balik tempahan. Sila tambah label dalam pesanan <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki>',
	'cite_error_references_no_text' => 'Tag <code>&lt;ref&gt;</code> tidak sah; teks bagi rujukan <code>$1</code> tidak disediakan',
	'cite_error_included_ref' => 'Tag &lt;ref&gt; tidak ditutup dengan &lt;/ref&gt;',
);

/** Low German (Plattdüütsch)
 * @author Slomox
 */
$messages['nds'] = array(
	'cite_desc' => 'Föögt <nowiki><ref[ name=id]></nowiki> un <nowiki><references/></nowiki> Tags för Zitaten to',
	'cite_croak' => 'Fehler bi de Referenzen. $1: $2',
	'cite_error_key_str_invalid' => 'Internen Fehler: ungülligen $str un/oder $key. Dat schull egentlich nie vörkamen.',
	'cite_error_stack_invalid_input' => 'Internen Fehler: ungülligen Stack-Slötel. Dat schull egentlich nie vörkamen.',
	'cite_error' => 'Zitat-Fehler: $1',
	'cite_error_ref_numeric_key' => 'Ungülligen Tag <tt>&lt;ref&gt;</tt>: de Naam dröff keen reine Tall wesen, bruuk en Naam, de de Saak beschrifft.',
	'cite_error_ref_no_key' => 'Ungülligen Tag <tt>&lt;ref&gt;</tt>: „ref“ ahn Inholt mutt en Naam hebben.',
	'cite_error_ref_too_many_keys' => 'Ungülligen Tag <tt>&lt;ref&gt;</tt>: ungüllige Naams, to’n Bispeel to veel.',
	'cite_error_ref_no_input' => 'Ungülligen Tag <tt>&lt;ref&gt;</tt>: „ref“ ahn Naam mutt en Inholt hebben.',
	'cite_error_references_invalid_input' => 'Ungülligen Tag <code>&lt;references&gt;</code>: Tosätzlichen Text is nich verlöövt, bruuk <tt>&lt;references /&gt;</tt>',
	'cite_error_references_invalid_parameters' => 'Ungülligen Tag <code>&lt;references&gt;</code>: Parameters sünd nich verlöövt, bruuk <tt>&lt;references /&gt;</tt>',
	'cite_error_references_invalid_parameters_group' => 'Ungülligen Tag <code>&lt;references&gt;</code>: Blot de Parameter „group“ is verlöövt, bruuk <tt>&lt;references /&gt;</tt> oder <tt>&lt;references group="…" /&gt;</tt>',
	'cite_error_references_no_backlink_label' => 'De verföögboren Tekens för de Lenken op Referenzen sünd all. Dat lett sik repareren, wenn in de Systemnaricht <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki> mehr Tekens angeven warrt.',
	'cite_error_references_no_text' => 'Ungülligen Tag <tt>&lt;ref&gt;</tt>; is keen Text för Refs mit den Naam <tt>$1</tt> angeven.',
);

/** Dutch (Nederlands)
 * @author Siebrand
 */
$messages['nl'] = array(
	'cite_desc' => 'Voegt <nowiki><ref[ name=id]></nowiki> en <nowiki><references/></nowiki> tags toe voor citaten',
	'cite_croak' => 'Probleem met Cite; $1: $2',
	'cite_error_key_str_invalid' => 'Interne fout;
onjuiste $str and/of $key.
Dit zou niet voor moeten komen.',
	'cite_error_stack_invalid_input' => 'Interne fout;
onjuiste stacksleutel.
Dit zou niet voor moeten komen.',
	'cite_error' => 'Citefout: $1',
	'cite_error_ref_numeric_key' => 'Onjuiste tag <code>&lt;ref&gt;</code>;
de naam kan geen eenvoudige integer zijn.
Gebruik een beschrijvende titel',
	'cite_error_ref_no_key' => 'Onjuiste tag <code>&lt;ref&gt;</code>;
refs zonder inhoud moeten een naam hebben',
	'cite_error_ref_too_many_keys' => 'Onjuiste tag <code>&lt;ref&gt;</code>;
onjuiste namen, bijvoorbeeld te veel',
	'cite_error_ref_no_input' => 'Onjuiste tag <code>&lt;ref&gt;</code>;
refs zonder naam moeten inhoud hebben',
	'cite_error_references_invalid_input' => 'Onjuiste tag <code>&lt;references&gt;</code>;
invoer is niet toegestaan, gebruik <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'Onjuiste tag <code>&lt;references&gt;</code>;
parameters zijn niet toegestaan.
Gebruik <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters_group' => 'Onjuiste tag <code>&lt;references&gt;</code>;
alleen de parameter "group" is toegestaan.
Gebruik <code>&lt;references /&gt;</code>, of <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => 'Het aantal beschikbare backlinklabels is opgebruikt.
Geef meer labels op in het bericht <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki>',
	'cite_error_references_no_text' => 'Onjuiste tag <code>&lt;ref&gt;</code>;
er is geen tekst opgegeven voor refs met de naam <code>$1</code>',
	'cite_error_included_ref' => 'Het sluitteken &lt;/ref&gt; mist voor de tag &lt;ref&gt;',
);

/** Norwegian Nynorsk (‪Norsk (nynorsk)‬)
 * @author Frokor
 */
$messages['nn'] = array(
	'cite_desc' => 'Legg til <nowiki><ref[ name=id]></nowiki> og <nowiki><references/></nowiki>-tagger for referansar',
	'cite_croak' => 'Feil i fotnotesystemet; $1: $2',
	'cite_error_key_str_invalid' => 'Intern feil: Ugyldig $str og/eller $key. Dette burde aldri skjedd.',
	'cite_error_stack_invalid_input' => 'Intern feil; ugyldig stakknøkkel. Dette burde aldri skjedd.',
	'cite_error' => 'Referansefeil: $1',
	'cite_error_ref_numeric_key' => 'Ugyldig <code>&lt;ref&gt;</code>-kode; namnet kan ikkje vere eit enkelt heiltal, bruk ein skildrande tittel',
	'cite_error_ref_no_key' => 'Ugyldig <code>&lt;ref&gt;</code>-kode; referansar utan innhald må innehalde namn',
	'cite_error_ref_too_many_keys' => 'Ugyldig <code>&lt;ref&gt;</code>-kode; ugyldige namn, t.d. for mange',
	'cite_error_ref_no_input' => 'Ugyldig <code>&lt;ref&gt;</code>-kode; referansar uten namn må ha innhald',
	'cite_error_references_invalid_input' => 'Ugyldig <code>&lt;references&gt;</code>-tagg: Innhald er ikkje tillete, bruk i staden <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'Ugyldig <code>&lt;references&gt;</code>-kode; ingen parametrar er tillat, bruk <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters_group' => 'Ugyldig <code>&lt;references&gt;</code>-tagg; berre parameteren «group» er tillatt. Bruk <code>&lt;references /&gt;</code> eller <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => 'Gjekk tom for eigendefinerte tilbakelenketekstar.
Definer fleire i meldinga <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki>',
	'cite_error_references_no_text' => 'Ugyldig <code>&lt;ref&gt;</code>-tagg; ingen tekst vart gjeve for referansen med namnet <code>$1</code>',
	'cite_error_included_ref' => 'Avsluttande &lt;/ref&gt;-tagg manglar for &lt;ref&gt;',
	'cite_reference_link_key_with_num' => '$1_$2',
);

/** Norwegian (bokmål)‬ (‪Norsk (bokmål)‬)
 * @author Jon Harald Søby
 */
$messages['no'] = array(
	'cite_desc' => 'Legger til <nowiki><ref[ name=id]></nowiki> og <nowiki><references/></nowiki>-tagger for referanser',
	'cite_croak' => 'Sitering døde; $1: $2',
	'cite_error_key_str_invalid' => 'Intern feil: Ugyldig $str og/eller $key. Dette burde aldri forekomme.',
	'cite_error_stack_invalid_input' => 'Intern feil; ugyldig stakknøkkel. Dette burde aldri forekomme.',
	'cite_error' => 'Siteringsfeil: $1',
	'cite_error_ref_numeric_key' => 'Ugyldig <code>&lt;ref&gt;</code>-kode; navnet kan ikke være et enkelt heltall, bruk en beskrivende tittel',
	'cite_error_ref_no_key' => 'Ugyldig <code>&lt;ref&gt;</code>-kode; referanser uten innhold må inneholde navn',
	'cite_error_ref_too_many_keys' => 'Ugyldig <code>&lt;ref&gt;</code>-kode; ugyldige navn, f.eks. for mange',
	'cite_error_ref_no_input' => 'Ugyldig <code>&lt;ref&gt;</code>-kode; referanser uten navn må ha innhold',
	'cite_error_references_invalid_input' => 'Ugyldig <code>&lt;references&gt;</code>-tagg: Innhold er ikke tillatt, bruk i stedet <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'Ugyldig <code>&lt;references&gt;</code>-kode; ingen parametere tillates, bruk <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters_group' => 'Ugyldig <code>&lt;references&gt;</code>-tagg; kun parameteret «group» tillates. Bruk <code>&lt;references /&gt;</code> eller <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => "Gikk tom for egendefinerte tilbakelenketekster; definer flere i beskjeden «''cite_references_link_many_format_backlink_labels''»",
	'cite_error_references_no_text' => 'Ugyldig <code>&lt;ref&gt;</code>-tagg; ingen tekst ble oppgitt for referansen ved navn <code>$1</code>',
	'cite_error_included_ref' => 'Avsluttende &lt;/ref&gt;-tagg mangler for &lt;ref&gt;',
	'cite_references_link_many_format_backlink_labels' => 'a b c d e f g h i j k l m n o p q r s t u v w x y z æ ø å aa ab ac ad ae af ag ah ai aj ak al am an ao ap aq ar as at au av aw ax ay az aæ aø aå ba bb bc bd be bf bg bh bi bj bk bl bm bn bo bp bq br bs bt bu bv bw bx by bz bæ bø bå ca cb cc cd ce cf cg ch ci cj ck cl cm cn co cp cq cr cs ct cu cv cw cx cy cz cæ cø cå da db dc dd de df dg dh di dj dk dl dm dn do dp dq dr ds dt du dv dw dx dy dz dæ dø då ea eb ec ed ee ef eg eh ei ej ek el em en eo ep eq er es et eu ev ew ex ey ez eæ eø eå fa fb fc fd fe ff fg fh fi fj fk fl fm fn fo fp fq fr fs ft fu fv fw fx fy fz fæ fø få ga gb gc gd ge gf gg gh gi gj gk gl gm gn go gp gq gr gs gt gu gv gw gx gy gz gæ gø gå ha hb hc hd he hf hg hh hi hj hk hl hm hn ho hp hq hr hs ht hu hv hw hx hy hz hæ hø hå ia ib ic id ie if ig ih ii ij ik il im in io ip iq ir is it iu iv iw ix iy iz iæ iø iå ja jb jc jd je jf jg jh ji jj jk jl jm jn jo jp jq jr js jt ju jv jw jx jy jz jæ jø jå ka kb kc kd ke kf kg kh ki kj kk kl km kn ko kp kq kr ks kt ku kv kw kx ky kz kæ kø kå la lb lc ld le lf lg lh li lj lk ll lm ln lo lp lq lr ls lt lu lv lw lx ly lz læ lø lå ma mb mc md me mf mg mh mi mj mk ml mm mn mo mp mq mr ms mt mu mv mw mx my mz mæ mø må na nb nc nd ne nf ng nh ni nj nk nl nm nn no np nq nr ns nt nu nv nw nx ny nz næ nø nå oa ob oc od oe of og oh oi oj ok ol om on oo op oq or os ot ou ov ow ox oy oz oæ oø oå pa pb pc pd pe pf pg ph pi pj pk pl pm pn po pp pq pr ps pt pu pv pw px py pz pæ pø på qa qb qc qd qe qf qg qh qi qj qk ql qm qn qo qp qq qr qs qt qu qv qw qx qy qz qæ qø qå ra rb rc rd re rf rg rh ri rj rk rl rm rn ro rp rq rr rs rt ru rv rw rx ry rz ræ rø rå sa sb sc sd se sf sg sh si sj sk sl sm sn so sp sq sr ss st su sv sw sx sy sz sæ sø så ta tb tc td te tf tg th ti tj tk tl tm tn to tp tq tr ts tt tu tv tw tx ty tz tæ tø tå ua ub uc ud ue uf ug uh ui uj uk ul um un uo up uq ur us ut uu uv uw ux uy uz uæ uø uå va vb vc vd ve vf vg vh vi vj vk vl vm vn vo vp vq vr vs vt vu vv vw vx vy vz væ vø vå wa wb wc wd we wf wg wh wi wj wk wl wm wn wo wp wq wr ws wt wu wv ww wx wy wz wæ wø wå xa xb xc xd xe xf xg xh xi xj xk xl xm xn xo xp xq xr xs xt xu xv xw xx xy xz xæ xø xå ya yb yc yd ye yf yg yh yi yj yk yl ym yn yo yp yq yr ys yt yu yv yw yx yy yz yæ yø yå za zb zc zd ze zf zg zh zi zj zk zl zm zn zo zp zq zr zs zt zu zv zw zx zy zz zæ zø zå æa æb æc æd æe æf æg æh æi æj æk æl æm æn æo æp æq ær æs æt æu æv æw æx æy æz ææ æø æå øa øb øc ød øe øf øg øh øi øj øk øl øm øn øo øp øq ør øs øt øu øv øw øx øy øz øæ øø øå åa åb åc åd åe åf åg åh åi åj åk ål åm ån åo åp åq år ås åt åu åv åw åx åy åz åæ åø åå',
);

/** Occitan (Occitan)
 * @author Cedric31
 */
$messages['oc'] = array(
	'cite_desc' => 'Apond las balisas <nowiki><ref[ name=id]></nowiki> e <nowiki><references/></nowiki>, per las citacions.',
	'cite_croak' => 'Citacion corrompuda ; $1 : $2',
	'cite_error_key_str_invalid' => 'Error intèrna ; $str esperada',
	'cite_error_stack_invalid_input' => 'Error intèrna ; clau de pila invalida',
	'cite_error' => 'Error de citacion : $1',
	'cite_error_ref_numeric_key' => 'Ampèl invalid ; clau non-integrala esperada',
	'cite_error_ref_no_key' => 'Ampèl invalid ; cap de clau pas especificada',
	'cite_error_ref_too_many_keys' => 'Ampèl invalid ; claus invalidas, per exemple, tròp de claus especificadas o clau erronèa',
	'cite_error_ref_no_input' => 'Ampèl invalid ; cap de dintrada pas especificada',
	'cite_error_references_invalid_input' => 'Entrada invalida ; entrada esperada',
	'cite_error_references_invalid_parameters' => 'Arguments invalids ; argument esperat',
	'cite_error_references_invalid_parameters_group' => 'Balisa <code>&lt;references&gt;</code> incorrècta ;

sol lo paramètre « group » es autorizat.

Utilizatz <code>&lt;references /&gt;</code>, o alara <code>&lt;references group="..." /&gt;</code>.',
	'cite_error_references_no_backlink_label' => 'Execucion en defòra de las etiquetas personalizadas, definissetz mai dins lo messatge <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki>',
	'cite_error_references_no_text' => 'Balisa  <code>&lt;ref&gt;</code> incorrècta ;

pas de tèxt per las referéncias nomenadas <code>$1</code>.',
	'cite_error_included_ref' => 'Clausura &lt;/ref&gt; omesa per la balisa &lt;ref&gt;',
	'cite_references_link_many_format_backlink_labels' => 'a á à b c ç d e é è f g h i í ì ï j k l m n o ó ò p q r s t u ú ù ü v w x y z',
);

/** Pangasinan (Pangasinan) */
$messages['pag'] = array(
	'cite_error' => 'Bitlaen so error $1; $2',
);

/** Polish (Polski)
 * @author Derbeth
 * @author Holek
 * @author Leinad
 * @author Sp5uhe
 */
$messages['pl'] = array(
	'cite_desc' => 'Dodaje znaczniki <nowiki><ref[ name=id]></nowiki> i <nowiki><references/></nowiki> ułatwiające podawanie źródeł cytatów',
	'cite_croak' => 'Cytowanie nieudane; $1: $2',
	'cite_error_key_str_invalid' => 'Błąd wewnętrzny – nieprawidłowy tekst $str i/lub klucz $key. To nigdy nie powinno się zdarzyć.',
	'cite_error_stack_invalid_input' => 'Błąd wewnętrzny – nieprawidłowy klucz sterty. To nigdy nie powinno się zdarzyć.',
	'cite_error' => "Błąd rozszerzenia ''cite'': $1",
	'cite_error_ref_numeric_key' => 'Nieprawidłowy znacznik <code>&lt;ref&gt;</code>. Nazwa nie może być liczbą, użyj nazwy opisowej.',
	'cite_error_ref_no_key' => 'Nieprawidłowy znacznik <code>&lt;ref&gt;</code>. Odnośnik ref z zawartością musi mieć nazwę.',
	'cite_error_ref_too_many_keys' => 'Nieprawidłowe nazwy parametrów elementu <code>&lt;ref&gt;</code>.',
	'cite_error_ref_no_input' => 'Bład w składni elementu <code>&lt;ref&gt;</code>. Przypisy bez podanej nazwy muszą posiadać treść',
	'cite_error_references_invalid_input' => 'Bład w składni elementu <code>&lt;references&gt;</code>. Nie można wprowadzać treści w tym elemencie, użyj <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'Bład w składni elementu <code>&lt;references&gt;</code>. Nie można wprowadzać parametrów do tego elementu, użyj <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters_group' => 'Nieprawidłowy znacznik <code>&lt;references&gt;</code>;
dostępny jest wyłącznie parametr „group”.
Użyj znacznika <code>&lt;references /&gt;</code>, lub <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => 'Zabrakło etykiet do przypisów.
Zadeklaruj więcej w komunikacie <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki>',
	'cite_error_references_no_text' => 'Bład w składni elementu <code>&lt;ref&gt;</code>. Brak tekstu w przypisie o nazwie <code>$1</code>',
	'cite_error_included_ref' => 'Brak tagu zamykającego &lt;/ref&gt; po otwartym tagu &lt;ref&gt;',
	'cite_references_link_many_format_backlink_labels' => 'a b c d e f g h i j k l m n o p q r s t u v w x y z aa ab ac ad ae af ag ah ai aj ak al am an ao ap aq ar as at au av aw ax ay az ba bb bc bd be bf bg bh bi bj bk bl bm bn bo bp bq br bs bt bu bv bw bx by bz ca cb cc cd ce cf cg ch ci cj ck cl cm cn co cp cq cr cs ct cu cv cw cx cy cz da db dc dd de df dg dh di dj dk dl dm dn do dp dq dr ds dt du dv dw dx dy dz ea eb ec ed ee ef eg eh ei ej ek el em en eo ep eq er es et eu ev ew ex ey ez fa fb fc fd fe ff fg fh fi fj fk fl fm fn fo fp fq fr fs ft fu fv fw fx fy fz ga gb gc gd ge gf gg gh gi gj gk gl gm gn go gp gq gr gs gt gu gv gw gx gy gz ha hb hc hd he hf hg hh hi hj hk hl hm hn ho hp hq hr hs ht hu hv hw hx hy hz ia ib ic id ie if ig ih ii ij ik il im in io ip iq ir is it iu iv iw ix iy iz ja jb jc jd je jf jg jh ji jj jk jl jm jn jo jp jq jr js jt ju jv jw jx jy jz ka kb kc kd ke kf kg kh ki kj kk kl km kn ko kp kq kr ks kt ku kv kw kx ky kz la lb lc ld le lf lg lh li lj lk ll lm ln lo lp lq lr ls lt lu lv lw lx ly lz ma mb mc md me mf mg mh mi mj mk ml mm mn mo mp mq mr ms mt mu mv mw mx my mz na nb nc nd ne nf ng nh ni nj nk nl nm nn no np nq nr ns nt nu nv nw nx ny nz oa ob oc od oe of og oh oi oj ok ol om on oo op oq or os ot ou ov ow ox oy oz pa pb pc pd pe pf pg ph pi pj pk pl pm pn po pp pq pr ps pt pu pv pw px py pz qa qb qc qd qe qf qg qh qi qj qk ql qm qn qo qp qq qr qs qt qu qv qw qx qy qz ra rb rc rd re rf rg rh ri rj rk rl rm rn ro rp rq rr rs rt ru rv rw rx ry rz sa sb sc sd se sf sg sh si sj sk sl sm sn so sp sq sr ss st su sv sw sx sy sz ta tb tc td te tf tg th ti tj tk tl tm tn to tp tq tr ts tt tu tv tw tx ty tz ua ub uc ud ue uf ug uh ui uj uk ul um un uo up uq ur us ut uu uv uw ux uy uz va vb vc vd ve vf vg vh vi vj vk vl vm vn vo vp vq vr vs vt vu vv vw vx vy vz wa wb wc wd we wf wg wh wi wj wk wl wm wn wo wp wq wr ws wt wu wv ww wx wy wz xa xb xc xd xe xf xg xh xi xj xk xl xm xn xo xp xq xr xs xt xu xv xw xx xy xz ya yb yc yd ye yf yg yh yi yj yk yl ym yn yo yp yq yr ys yt yu yv yw yx yy yz za zb zc zd ze zf zg zh zi zj zk zl zm zn zo zp zq zr zs zt zu zv zw zx zy zz',
);

/** Portuguese (Português)
 * @author 555
 * @author Malafaya
 */
$messages['pt'] = array(
	'cite_desc' => 'Adiciona marcas <nowiki><ref[ name=id]></nowiki> e <nowiki><references/></nowiki> para citações',
	'cite_croak' => 'Citação com problemas; $1: $2',
	'cite_error_key_str_invalid' => 'Erro interno; $str inválido',
	'cite_error_stack_invalid_input' => 'Erro interno; chave fixa inválida',
	'cite_error' => 'Erro de citação $1',
	'cite_error_ref_numeric_key' => 'Código <code>&lt;ref&gt;</code> inválido; o nome não pode ser um número. Utilize um nome descritivo',
	'cite_error_ref_no_key' => 'Código <code>&lt;ref&gt;</code> inválido; refs sem conteúdo devem ter um parâmetro de nome',
	'cite_error_ref_too_many_keys' => 'Código <code>&lt;ref&gt;</code> inválido; nomes inválidos (por exemplo, nome muito extenso)',
	'cite_error_ref_no_input' => 'Código <code>&lt;ref&gt;</code> inválido; refs sem parâmetro de nome devem possuir conteúdo a elas associado',
	'cite_error_references_invalid_input' => 'Código <code>&lt;references&gt;</code> inválido; no input is allowed, use
<code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'Código <code>&lt;references&gt;</code> inválido; não são permitidos parâmetros. Utilize como <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters_group' => 'Marca <code>&lt;references&gt;</code> inválida;
só o parâmetro "group" é permitido.
Use <code>&lt;references /&gt;</code>, ou <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => "Etiquetas de backlink esgotadas. Defina mais na mensagem \"''cite_references_link_many_format_backlink_labels''\"",
	'cite_error_references_no_text' => 'Tag <code>&lt;ref&gt;</code> inválida; não foi fornecido texto para as refs chamadas <code>$1</code>',
	'cite_error_included_ref' => '&lt;/ref&gt; de fecho em falta para a marca &lt;ref&gt;',
);

/** Romanian (Română)
 * @author KlaudiuMihaila
 * @author Mihai
 */
$messages['ro'] = array(
	'cite_desc' => 'Adaugă etichete <nowiki><ref[ name=id]></nowiki> şi <nowiki><references/></nowiki>, pentru citări',
	'cite_croak' => 'Citare coruptă; $1 : $2',
	'cite_error_key_str_invalid' => 'Eroare internă;
$str invalid sau/şi $key.
Acestea nu ar trebui să se întâmple.',
	'cite_error_stack_invalid_input' => 'Eroare internă;
stivă cheie invalidă.
Acestea nu ar trebui să se întâmple.',
	'cite_error' => 'Eroare la citare: $1',
	'cite_error_ref_numeric_key' => 'Etichetă <code>&lt;ref&gt;</code> invalidă;
numele nu poate fi un număr. Foloseşte un titlu descriptiv',
	'cite_error_ref_no_key' => 'Etichetă <code>&lt;ref&gt;</code> invalidă;
ref-urile fără conţinut trebuie să aibă un nume',
	'cite_error_ref_too_many_keys' => 'Etichetă <code>&lt;ref&gt;</code> invalidă;
nume invalid, ex. prea multe nume',
	'cite_error_ref_no_input' => 'Etichetă <code>&lt;ref&gt;</code> invalidă;
ref-urile fără nume trebuie să aibă conţinut',
	'cite_error_references_invalid_input' => 'Etichetă <code>&lt;references&gt;</code> invalidă;
nu sunt permise intrări. Foloseşte eticheta <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'Etichetă <code>&lt;references&gt;</code> invalidă;
parametrii nu sunt permişi.
Foloseşte eticheta <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters_group' => 'Etichetă <code>&lt;references&gt;</code> invalidă;
doar parametrul „grup” este permis.
Foloseşte eticheta <code>&lt;references /&gt;</code>, sau <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_text' => 'Etichetă <code>&lt;ref&gt;</code> invalidă;
niciun text nu a fost furnizat pentru ref-urile numite <code>$1</code>',
	'cite_error_included_ref' => 'Adaugă &lt;/ref&gt; eticheta a fost deschisă prin &lt;ref&gt;',
);

/** Russian (Русский)
 * @author Ahonc
 * @author Kaganer
 * @author Kalan
 * @author Александр Сигачёв
 */
$messages['ru'] = array(
	'cite_desc' => 'Добавляет теги <nowiki><ref[ name=id]></nowiki> и <nowiki><references/></nowiki> для сносок',
	'cite_croak' => 'Цитата сдохла; $1: $2',
	'cite_error_key_str_invalid' => 'Внутренняя ошибка: неверный $str',
	'cite_error_stack_invalid_input' => 'Внутренняя ошибка.
Неверный ключ стека.
Это никогда не должно происходить.',
	'cite_error' => 'Ошибка цитирования $1',
	'cite_error_ref_numeric_key' => 'Неправильный вызов: ожидался нечисловой ключ',
	'cite_error_ref_no_key' => 'Неправильный вызов: ключ не был указан',
	'cite_error_ref_too_many_keys' => 'Неправильный вызов: неверные ключи, например было указано слишком много ключей или ключ был неправильным',
	'cite_error_ref_no_input' => 'Неверный вызов: нет входных данных',
	'cite_error_references_invalid_input' => 'Входные данные недействительны, так как не предполагаются',
	'cite_error_references_invalid_parameters' => 'Переданы недействительные параметры; их вообще не предусмотрено.',
	'cite_error_references_invalid_parameters_group' => 'Ошибочный тег <code>&lt;references&gt;</code>;
можно использовать только параметр «group».
Используйте <code>&lt;references /&gt;</code>, или <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => 'Не хватает символов для возвратных гиперссылок.
Следует расширить системное сообщение <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki>',
	'cite_error_references_no_text' => 'Неверный тег <code>&lt;ref&gt;</code>; для сносок <code>$1</code> не указан текст',
	'cite_error_included_ref' => 'Отсутствует закрывающий тег &lt;/ref&gt;',
	'cite_references_link_many_format_backlink_labels' => 'а б в г д е ё ж з и й к л м н о п р с т у ф х ц ч ш щ ъ ы ь э ю я аа аб ав аг ад ае ё аж аз аи ай ак ал ам ан ао ап ар ас ат ау аф ах ац ач аш ащ аъ аы аь аэ аю ая ба бб бв бг бд бе бж бз би бй бк бл бм бн бо бп бр бс бт бу бф бх бц бч бш бщ бъ бы бь бэ бю бя ва вб вв вг вд ве вж вз ви вй вк вл вм вн во вп вр вс вт ву вф вх вц вч вш вщ въ вы вь вэ вю вя га гб гв гг гд ге гж гз ги гй гк гл гм гн го гп гр гс гт гу гф гх гц гч гш гщ гъ гы гь гэ гю гя да дб дв дг дд де дж дз ди дй дк дл дм дн до дп др дс дт ду дф дх дц дч дш дщ дъ ды дь дэ дю дя еа еб ев ег ед ее еж ез еи ей ек ел ем ен ео еп ер ес ет еу еф ех ец еч еш ещ еъ еы еь еэ ею ея жа жб жв жг жд же жж жз жи жй жк жл жм жн жо жп жр жс жт жу жф жх жц жч жш жщ жъ жы жь жэ жю жя за зб зв зг зд зе зж зз зи зй зк зл зм зн зо зп зр зс зт зу зф зх зц зч зш зщ зъ зы зь зэ зю зя иа иб ив иг ид ие иж из ии ий ик ил им ин ио ип ир ис ит иу иф их иц ич иш ищ иъ иы иь иэ ию ия йа йб йв йг йд йе йж йз йи йй йк йл йм йн йо йп йр йс йт йу йф йх йц йч йш йщ йъ йы йь йэ йю йя ка кб кв кг кд ке кж кз ки кй кк кл км кн ко кп кр кс кт ку кф кх кц кч кш кщ къ кы кь кэ кю кя ла лб лв лг лд ле лж лз ли лй лк лл лм лн ло лп лр лс лт лу лф лх лц лч лш лщ лъ лы ль лэ лю ля ма мб мв мг мд ме мж мз ми мй мк мл мм мн мо мп мр мс мт му мф мх мц мч мш мщ мъ мы мь мэ мю мя на нб нв нг нд не нж нз ни нй нк нл нм нн но нп нр нс нт ну нф нх нц нч нш нщ нъ ны нь нэ ню ня оа об ов ог од ое ож оз ои ой ок ол ом он оо оп ор ос от оу оф ох оц оч ош ощ оъ оы оь оэ ою оя па пб пв пг пд пе пж пз пи пй пк пл пм пн по пп пр пс пт пу пф пх пц пч пш пщ пъ пы пь пэ пю пя ра рб рв рг рд ре рж рз ри рй рк рл рм рн ро рп рр рс рт ру рф рх рц рч рш рщ ръ ры рь рэ рю ря са сб св сг сд се сж сз си сй ск сл см сн со сп ср сс ст су сф сх сц сч сш сщ съ сы сь сэ сю ся та тб тв тг тд те тж тз ти тй тк тл тм тн то тп тр тс тт ту тф тх тц тч тш тщ тъ ты ть тэ тю тя уа уб ув уг уд уе уж уз уи уй ук ул ум ун уо уп ур ус ут уу уф ух уц уч уш ущ уъ уы уь уэ ую уя фа фб фв фг фд фе фж фз фи фй фк фл фм фн фо фп фр фс фт фу фф фх фц фч фш фщ фъ фы фь фэ фю фя ха хб хв хг хд хе хж хз хи хй хк хл хм хн хо хп хр хс хт ху хф хх хц хч хш хщ хъ хы хь хэ хю хя ца цб цв цг цд це цж цз ци цй цк цл цм цн цо цп цр цс цт цу цф цх цц цч цш цщ цъ цы ць цэ цю ця ча чб чв чг чд че чж чз чи чй чк чл чм чн чо чп чр чс чт чу чф чх чц чч чш чщ чъ чы чь чэ чю чя ша шб шв шг шд ше шж шз ши шй шк шл шм шн шо шп шр шс шт шу шф шх шц шч шш шщ шъ шы шь шэ шю шя ща щб щв щг щд ще щж щз щи щй щк щл щм щн що щп щр щс щт щу щф щх щц щч щш щщ щъ щы щь щэ щю щя ъа ъб ъв ъг ъд ъе ъж ъз ъи ъй ък ъл ъм ън ъо ъп ър ъс ът ъу ъф ъх ъц ъч ъш ъщ ъъ ъы ъь ъэ ъю ъя ыа ыб ыв ыг ыд ые ыж ыз ыи ый ык ыл ым ын ыо ып ыр ыс ыт ыу ыф ых ыц ыч ыш ыщ ыъ ыы ыь ыэ ыю ыя ьа ьб ьв ьг ьд ье ьж ьз ьи ьй ьк ьл ьм ьн ьо ьп ьр ьс ьт ьу ьф ьх ьц ьч ьш ьщ ьъ ьы ьь ьэ ью ья эа эб эв эг эд эе эж эз эи эй эк эл эм эн эо эп эр эс эт эу эф эх эц эч эш эщ эъ эы эь ээ эю эя юа юб юв юг юд юе юж юз юи юй юк юл юм юн юо юп юр юс ют юу юф юх юц юч юш ющ юъ юы юь юэ юю юя яа яб яв яг яд яе яж яз яи яй як ял ям ян яо яп яр яс ят яу яф ях яц яч яш ящ яъ яы яь яэ яю яя',
);

/** Yakut (Саха тыла)
 * @author HalanTul
 */
$messages['sah'] = array(
	'cite_desc' => 'Хос быһаарыы <nowiki><ref[ name=id]></nowiki> уонна <nowiki><references/></nowiki> тиэктэрин эбэр',
	'cite_croak' => 'Быһа тардыы суох буолбут (Цитата сдохла); $1: $2',
	'cite_error_key_str_invalid' => 'Иһинээҕи сыыһа: $str уонна/эбэтэр $key сыыһалар.',
	'cite_error_stack_invalid_input' => 'Иһинээҕи сыыһа: stack key сыыһалаах',
	'cite_error' => 'Цитата сыыһата: $1',
	'cite_error_ref_numeric_key' => 'Неправильный вызов: ожидался нечисловой ключ',
	'cite_error_ref_no_key' => '<code>&lt;ref&gt;</code> тиэк алҕаһа (Неправильный вызов): аата (күлүүһэ) ыйыллыбатах',
	'cite_error_ref_too_many_keys' => '<code>&lt;ref&gt;</code> тиэк алҕаһа (Неправильный вызов): аата сыыһа ыйыллыбыт, эбэтэр наһаа элбэх аат суруллубут',
	'cite_error_ref_no_input' => '<code>&lt;ref&gt;</code> тиэк алҕастаах (Неверный вызов): иһинээҕитэ сыыһа',
	'cite_error_references_invalid_input' => '<code>&lt;references&gt;</code> тиэк алҕаһа, иһигэр туох да суох буолуохтаах',
	'cite_error_references_invalid_parameters' => 'Сыыһа параметрдар бэриллибиттэр; <code>&lt;references /&gt;</code> тиэккэ отой суох буолуохтаахтар',
	'cite_error_references_invalid_parameters_group' => 'Сыыһалаах <code>&lt;references&gt;</code> тиэк;
"group" эрэ парааматыры туһаныахха сөп.
Маны <code>&lt;references /&gt;</code>, эбэтэр <code>&lt;references group="..." /&gt;</code> туһан.',
	'cite_error_references_no_backlink_label' => 'Не хватает символов для возвратных гиперссылок; следует расширить системную переменную <nowiki>[[MediaWiki:Cite_references_link_many_format_backlink_labels]]</nowiki>',
	'cite_error_references_no_text' => 'Сыыһа тиэк (тег) <code>&lt;ref&gt;</code>; хос быһаарыыларга <code>$1</code> тиэкис ыйыллыбатах',
);

/** Sinhala (සිංහල)
 * @author නන්දිමිතුරු
 */
$messages['si'] = array(
	'cite_desc' => 'උපහරණයන් සඳහා, <nowiki><ref[ name=id]></nowiki> සහ <nowiki><references/></nowiki> ටැගයන්, එකතු කරයි',
	'cite_croak' => 'උපන්‍යාසය නිරුද්ධවිය; $1: $2',
	'cite_error_key_str_invalid' => 'අභ්‍යන්තර දෝෂය;
අනීතික  $str සහ/හෝ $key.
මෙය කිසිදින සිදුනොවිය යුතුය.',
	'cite_error_stack_invalid_input' => 'අභ්‍යන්තර දෝෂය;
අනීතික ඇසිරුම්  යතුර.
මෙය කිසිදින සිදුනොවිය යුතුය.',
	'cite_error' => 'උපන්‍යාස දෝෂය: $1',
	'cite_error_ref_numeric_key' => 'අනීතික <code>&lt;ref&gt;</code> ටැගය;
නම සරල  පූර්ණාංකයක් විය නොහැක. විස්තරශීලි ශිර්ෂයක් භාවිතා කරන්න',
	'cite_error_ref_no_key' => 'අනීතික <code>&lt;ref&gt;</code> ටැගය;
පෙළ විරහිත ආශ්‍රේය සඳහා නමක් තිබිය යුතුය',
	'cite_error_ref_too_many_keys' => 'අනීතික <code>&lt;ref&gt;</code> ටැගය;
අනීතික නාමයන්, නිද. පමණට වැඩි',
	'cite_error_ref_no_input' => 'අනීතික <code>&lt;ref&gt;</code> ටැගය;
නාමයක් නොමැති ආශ්‍රේය සඳහා පෙළක් තිබිය යුතුය',
	'cite_error_references_invalid_input' => 'අනීතික <code>&lt;references&gt;</code> ටැගය;
කිසිම ප්‍රදානයකට ඉඩ නොදෙයි. <code>&lt;references /&gt;</code> භාවිත කරන්න',
	'cite_error_references_invalid_parameters' => 'අනීතික <code>&lt;references&gt;</code> ටැගය;
කිසිම පරාමිතිකයකට ඉඩ නොදෙයි.
<code>&lt;references /&gt;</code> භාවිත කරන්න',
	'cite_error_references_invalid_parameters_group' => 'අනීතික <code>&lt;references&gt;</code> ටැගය;
"කාණ්ඩය" පරාමිතියට පමණක් ඉඩ දෙයි.
<code>&lt;references /&gt;</code>, හෝ <code>&lt;references group="..." /&gt;</code> භාවිත කරන්න',
	'cite_error_references_no_backlink_label' => 'උපයෝග්‍ය පසුසබැඳුම් ලේබල අවසාන විය.
<nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki> පණිවුඩයෙහි තවත් ඒවා අර්ථදක්වන්න',
	'cite_error_references_no_text' => 'අනීතික <code>&lt;ref&gt;</code> ටැගය;
<code>$1</code> නමැති ආශ්‍රේයන් සඳහා කිසිදු පෙළක් සපයා නොතිබුණි',
	'cite_error_included_ref' => '&lt;ref&gt; ටැගය සොයාගත නොහැකි බැවින් &lt;/ref&gt; වසා දමමින්',
	'cite_reference_link_key_with_num' => '$1_$2',
	'cite_reference_link_prefix' => 'උපන්‍යාස_යොමුව-',
	'cite_references_link_prefix' => 'උපන්‍යාස_සටහන-',
	'cite_reference_link' => '<sup id="$1" class="reference">[[#$2|<nowiki>[</nowiki>$3<nowiki>]</nowiki>]]</sup>',
	'cite_references_link_one' => '<li id="$1">[[#$2|↑]] $3</li>',
	'cite_references_link_many' => '<li id="$1">↑ $2 $3</li>',
	'cite_references_link_many_format' => '<sup>[[#$1|$2]]</sup>',
);

/** Slovak (Slovenčina)
 * @author Helix84
 */
$messages['sk'] = array(
	'cite_desc' => 'Pridáva značky <nowiki><ref[ name=id]></nowiki> a <nowiki><references/></nowiki> pre citácie',
	'cite_croak' => 'Citát je už neaktuálny; $1: $2',
	'cite_error_key_str_invalid' => 'Vnútorná chyba; neplatný $str',
	'cite_error_stack_invalid_input' => 'Vnútorná chyba; neplatný kľúč zásobníka',
	'cite_error' => 'Chyba citácie $1',
	'cite_error_ref_numeric_key' => 'Neplatné volanie; očakáva sa neceločíselný typ kľúča',
	'cite_error_ref_no_key' => 'Neplatné volanie; nebol špecifikovaný kľúč',
	'cite_error_ref_too_many_keys' => 'Neplatné volanie; neplatné kľúče, napr. príliš veľa alebo nesprávne špecifikovaný kľúč',
	'cite_error_ref_no_input' => 'Neplatné volanie; nebol špecifikovaný vstup',
	'cite_error_references_invalid_input' => 'Neplatné volanie; neočakával sa vstup',
	'cite_error_references_invalid_parameters' => 'Neplatné parametre; neočakávli sa žiadne',
	'cite_error_references_invalid_parameters_group' => 'Neplatná značka <code>&lt;references&gt;</code>;
je povolený iba parameter „group“.
Použite <code>&lt;references /&gt;</code> alebo <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => 'Minuli sa generované návestia spätných odkazov, definujte viac v správe <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki>',
	'cite_error_references_no_text' => 'Neplatná značka <code>&lt;ref&gt;</code>; nebol zadaný text pre referencie s názvom <code>$1</code>',
	'cite_error_included_ref' => 'Chýba zakončenie značky &lt;ref&gt; (&lt;/ref&gt;)',
);

/** Serbian Cyrillic ekavian (ћирилица)
 * @author Millosh
 */
$messages['sr-ec'] = array(
	'cite_desc' => 'Додаје <nowiki><ref[ name=id]></nowiki> и <nowiki><references/></nowiki> ознаке за цитирање.',
	'cite_croak' => 'Додатак за цитирање је умро; $1: $2.',
	'cite_error_key_str_invalid' => 'Унутрашња грешка; лош $str и/или $key. Ово не би требало никад да се деси.',
	'cite_error_stack_invalid_input' => 'Унутрашња грешка; лош кључ стека. Ово не би требало никад да се деси.',
	'cite_error' => 'Грешка цитата: $1',
	'cite_error_ref_numeric_key' => 'Лоша ознака <code>&lt;ref&gt;</code>; име не може бити једноставни интеџер. Користи описни наслов.',
	'cite_error_ref_no_key' => 'Лоша ознака <code>&lt;ref&gt;</code>; ref-ови без садржаја морају имати име.',
	'cite_error_ref_too_many_keys' => 'Лоша ознака <code>&lt;ref&gt;</code>; лоша имена, односно много њих.',
	'cite_error_ref_no_input' => 'Лоша ознака <code>&lt;ref&gt;</code>; ref-ови без имена морају имати садржај.',
	'cite_error_references_invalid_input' => 'Лоша ознака <code>&lt;references&gt;</code>; улаз није дозвољен. Користи <code>&lt;references /&gt;</code>.',
	'cite_error_references_invalid_parameters' => 'Лоша ознака <code>&lt;references&gt;</code>; параметри нису дозвољени. Користи <code>&lt;references /&gt;</code>.',
	'cite_error_references_invalid_parameters_group' => 'Лоша ознака <code>&lt;references&gt;</code>; само је парамтера "group" дозвољен. Користи <code>&lt;references /&gt;</code> или <code>&lt;references group="..."&gt;</code>.',
	'cite_error_references_no_backlink_label' => 'Нестале су посебне ознаке за задње везе. Одреди их више у поруци <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki>.',
	'cite_error_references_no_text' => 'Лоша ознака <code>&lt;ref&gt;</code>; нема текста за ref-ове под именом <code>$1</code>.',
);

/** Seeltersk (Seeltersk)
 * @author Pyt
 */
$messages['stq'] = array(
	'cite_desc' => 'Föiget foar Wällenätterwiese do <nowiki><ref[ name=id]></nowiki> un <nowiki><references/></nowiki> Tags tou',
	'cite_croak' => 'Failer in dät Referenz-System. $1: $2',
	'cite_error_key_str_invalid' => 'Internen Failer: ungultigen $str',
	'cite_error_stack_invalid_input' => 'Internen Failer: uungultigen „name“-stack. Dit schuul eegentelk goarnit passierje konne.',
	'cite_error' => 'Referenz-Failer $1',
	'cite_error_ref_numeric_key' => 'Uungultige <code><nowiki><ref></nowiki></code>-Ferweendenge: „name“ duur naan scheenen Taalenwäid weese, benutsje n beschrieuwenden Noome.',
	'cite_error_ref_no_key' => 'Uungultige <code><nowiki><ref></nowiki></code>-Ferweendenge: „ref“ sunner Inhoold mout n Noome hääbe.',
	'cite_error_ref_too_many_keys' => 'Uungultige <code><nowiki><ref></nowiki></code>-Ferweendenge: „name“ is uungultich of tou loang.',
	'cite_error_ref_no_input' => 'Uungultige <code><nowiki><ref></nowiki></code>-Ferweendenge: „ref“ sunner Noome mout n Inhoold hääbe.',
	'cite_error_references_invalid_input' => 'Uungultige <code><nowiki><references></nowiki></code>-Ferweendenge: Der is naan bietoukuumenden Text ferlööwed, ferweend bloot <code><nowiki><references /></nowiki></code>.',
	'cite_error_references_invalid_parameters' => 'Uungultige <code><nowiki><reference></nowiki></code>-Ferweendenge: Der sunt neen bietoukuumende Parametere ferlööwed, ferweend bloot <code><nowiki><reference /></nowiki></code>.',
	'cite_error_references_invalid_parameters_group' => 'Ungultige <code>&lt;references&gt;</code>-Ferweendenge: Bloot die Parameter „group“ is ferlööwed, ferweend <tt>&lt;references /&gt;</tt> of <tt>&lt;references group="…" /&gt;</tt>',
	'cite_error_references_no_backlink_label' => 'Ne Referenz fon ju Foarm <code><nowiki><ref name="…"/></nowiki></code> wäd oafter benutsed as Bouksteeuwen deer sunt. N Administrator mout <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki> uum wiedere Bouksteeuwen/Teekene ferfulständigje.',
	'cite_error_references_no_text' => 'Uungultigen <code>&lt;ref&gt;</code>-Tag; der wuude naan Text foar dät Ref mäd dän Noome <code>$1</code> anroat.',
	'cite_error_included_ref' => 'Der failt n sluutend &lt;/ref&gt;',
	'cite_reference_link_key_with_num' => '$1_$2',
	'cite_reference_link_prefix' => 'cite_ref-',
);

/** Sundanese (Basa Sunda)
 * @author Kandar
 */
$messages['su'] = array(
	'cite_desc' => 'Nambahkeun tag <nowiki><ref[ name=id]></nowiki> jeung <nowiki><references/></nowiki>, pikeun cutatan',
	'cite_error_key_str_invalid' => 'Kasalahan internal; salah $str jeung/atawa $key. Kuduna mah teu kieu.',
);

/** Swedish (Svenska)
 * @author Boivie
 * @author Lejonel
 * @author M.M.S.
 */
$messages['sv'] = array(
	'cite_desc' => 'Lägger till taggarna <nowiki><ref[ name=id]></nowiki> och <nowiki><references/></nowiki> för referenser till källor',
	'cite_croak' => 'Fel i fotnotssystemet; $1: $2',
	'cite_error_key_str_invalid' => 'Internt fel; $str eller $key är ogiltiga.  Det här borde aldrig hända.',
	'cite_error_stack_invalid_input' => 'Internt fel; ogiltig nyckel i stacken.  Det här borde aldrig hända.',
	'cite_error' => 'Referensfel: $1',
	'cite_error_ref_numeric_key' => "Ogiltig <code>&lt;ref&gt;</code>-tag; parametern 'name' kan inte vara ett tal, använd ett beskrivande namn",
	'cite_error_ref_no_key' => 'Ogiltig <code>&lt;ref&gt;</code>-tag; referenser utan innehåll måste ha ett namn',
	'cite_error_ref_too_many_keys' => "Ogiltig <code>&lt;ref&gt;</code>-tag; ogiltiga parametrar, den enda tillåtna parametern är 'name'",
	'cite_error_ref_no_input' => 'Ogiltig <code>&lt;ref&gt;</code>-tag; referenser utan namn måste ha innehåll',
	'cite_error_references_invalid_input' => 'Ogiltig <code>&lt;references&gt;</code>-tag; inget innehåll är tillåtet, använd <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'Ogiltig <code>&lt;references&gt;</code>-tag; inga parametrar tillåts, använd <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters_group' => 'Ogiltig <code>&lt;references&gt;</code>-tagg;
"group"-parametern är endast tillåten.
Använd <code>&lt;references /&gt;</code>, eller <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => 'De definierade etiketterna för tillbaka-länkar har tagit slut, definiera fler etiketter i systemmedelandet <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki>',
	'cite_error_references_no_text' => 'Ogiltig <code>&lt;ref&gt;</code>-tag; ingen text har angivits för referensen med namnet <code>$1</code>',
	'cite_error_included_ref' => 'Avslutande &lt;/ref&gt; saknas för &lt;ref&gt;-tagg',
);

/** Telugu (తెలుగు)
 * @author Chaduvari
 * @author Veeven
 */
$messages['te'] = array(
	'cite_desc' => 'ఉదహరింపులకు <nowiki><ref[ name=id]></nowiki> మరియు <nowiki><references/></nowiki> టాగులను చేర్చుతుంది',
	'cite_croak' => 'ఉదహరింపు చచ్చింది; $1: $2',
	'cite_error_key_str_invalid' => 'అంతర్గత పొరపాటు: తప్పుడు $str మరియు/లేదా $key. ఇది ఎప్పుడూ జరగకూడదు.',
	'cite_error_stack_invalid_input' => 'అంతర్గత పొరపాటు: తప్పుడు స్టాక్ కీ. ఇది ఎప్పుడూ జరగకూడదు.',
	'cite_error' => 'ఉదహరింపు పొరపాటు: $1',
	'cite_error_ref_numeric_key' => 'తప్పుడు <code>&lt;ref&gt;</code> టాగు; పేరు సరళ సంఖ్య అయివుండకూడదు, వివరమైన శీర్షిక వాడండి',
	'cite_error_ref_no_key' => 'సరైన <code>&lt;ref&gt;</code> ట్యాగు కాదు; విషయం లేని ref లకు తప్పనిసరిగా పేరొకటుండాలి',
	'cite_error_ref_too_many_keys' => 'సరైన <code>&lt;ref&gt;</code> ట్యాగు కాదు; తప్పు పేర్లు, ఉదాహరణకు మరీ ఎక్కువ',
	'cite_error_ref_no_input' => 'సరైన <code>&lt;ref&gt;</code> ట్యాగు కాదు; పేరు లేని ref లలో తప్పనిసరిగా విషయం ఉండాలి',
	'cite_error_references_invalid_input' => 'సరైన <code>&lt;references&gt;</code> ట్యాగు కాదు; ఇన్‌పుట్ కు అనుమతి లేదు, <code>&lt;references /&gt;</code> వాడండి.',
	'cite_error_references_invalid_parameters' => 'సరైన <code>&lt;references&gt;</code> ట్యాగు కాదు; పారామీటర్లకు కు అనుమతి లేదు, ఈ లోపాన్ని కలుగజేసే ఒక ఉదాహరణ: <references someparameter="value" />',
	'cite_error_references_no_backlink_label' => 'మీ స్వంత బ్యాక్‌లింకు లేబుళ్ళు అయిపోయాయి. <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki> సందేశంలో మరిన్ని లేబుళ్ళను నిర్వచించుకోండి.',
	'cite_error_references_no_text' => 'సరైన <code>&lt;ref&gt;</code> కాదు; <code>$1</code> అనే పేరుగల ref లకు పాఠ్యమేమీ ఇవ్వలేదు',
);

/** Tajik (Cyrillic) (Тоҷикӣ (Cyrillic))
 * @author Ibrahim
 */
$messages['tg-cyrl'] = array(
	'cite_desc' => 'Барчасбҳои <nowiki><ref[ name=id]></nowiki> ва <nowiki><references/></nowiki>  барои ёд кардан, изофа мекунад',
	'cite_croak' => 'Ёд кардан хароб шуд; $1: $2',
	'cite_error_key_str_invalid' => 'Хатои дохилӣ; $str ва/ё $key ғайримиҷоз.  Ин хато набояд ҳаргиз рух диҳад.',
	'cite_error_stack_invalid_input' => 'Хатои дохилӣ; клиди пушта ғайримиҷоз.  Ин хато набояд ҳаргиз рух диҳад.',
	'cite_error' => 'Хатои ёдкард: $1',
	'cite_error_ref_numeric_key' => 'Барчасби <code>&lt;ref&gt;</code> ғайримиҷоз; ном наметавонад як адад бошад, унвони возеҳтареро истифода кунед',
	'cite_error_ref_no_key' => 'Барчасби <code>&lt;ref&gt;</code> ғайримиҷоз; ёдкардҳо бидуни мӯҳтаво бояд ном дошта бошанд',
	'cite_error_ref_too_many_keys' => 'Барчасби  <code>&lt;ref&gt;</code> ғайримиҷоз; номҳои ғайримиҷоз ё беш аз андоза',
	'cite_error_ref_no_input' => 'Барчасби  <code>&lt;ref&gt;</code> ғайримиҷоз; ёдкардҳои бидуни ном бояд мӯҳтаво дошта бошанд',
	'cite_error_references_invalid_input' => 'Барчасби <code>&lt;references&gt;</code> ғайримиҷоз; вуруди матн миҷоз нест, аз
<code>&lt;references /&gt;</code> истифода кунед',
	'cite_error_references_invalid_parameters' => 'Барчасби <code>&lt;references&gt;</code> ғайримиҷоз; истифода аз параметр миҷоз аст, аз  <code>&lt;references /&gt;</code> истифода кунед',
	'cite_error_references_invalid_parameters_group' => 'Барчасби <code>&lt;references&gt;</code> номӯътабар;
параметри "гурӯҳ" танҳо иҷозашуда аст.
Барчасби <code>&lt;references /&gt;</code> ё <code>&lt;references group="..." /&gt;</code> -ро истифода баред',
	'cite_error_references_no_backlink_label' => 'Барчасбҳои пайванд ба интиҳо расид, мавориди ҷадидро дар пайём  <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki> истифода кунед',
	'cite_error_references_no_text' => 'Барчасби  <code>&lt;ref&gt;</code> ғайримиҷоз; матне барои ёдкардҳо бо номи <code>$1</code> ворид нашудааст',
);

/** Thai (ไทย)
 * @author Passawuth
 */
$messages['th'] = array(
	'cite_desc' => 'ใส่ <nowiki><ref[ name=id]></nowiki> และ <nowiki><references /></nowiki> สำหรับการอ้างอิง',
	'cite_croak' => 'แหล่งอ้างอิงเสีย; $1: $2',
	'cite_error' => 'อ้างอิงผิดพลาด: $1',
);

/** Turkish (Türkçe) */
$messages['tr'] = array(
	'cite_error' => 'Kaynak hatası $1',
);

/** Ukrainian (Українська)
 * @author AS
 * @author Ahonc
 */
$messages['uk'] = array(
	'cite_desc' => 'Додає теги <nowiki><ref[ name=id]></nowiki> і <nowiki><references/></nowiki> для виносок',
	'cite_croak' => 'Цитата померла; $1: $2',
	'cite_error_key_str_invalid' => 'Внутрішня помилка:
неправильний $str і/або $key.',
	'cite_error_stack_invalid_input' => 'Внутрішня помилка: неправильний ключ стека.',
	'cite_error' => 'Помилка цитування: $1',
	'cite_error_ref_numeric_key' => 'Неправильний виклик <code>&lt;ref&gt;</code>:
назва не може містити тільки цифри.',
	'cite_error_ref_no_key' => 'Неправильний виклик <code>&lt;ref&gt;</code>:
порожній тег <code>ref</code> повинен мати параметр name.',
	'cite_error_ref_too_many_keys' => 'Неправильний виклик <code>&lt;ref&gt;</code>:
вказані неправильні значення <code>name</code> або вказано багато прамаетрів',
	'cite_error_ref_no_input' => 'Неправильний виклик <code>&lt;ref&gt;</code>:
тег <code>ref</code> без назви повинен мати вхідні дані',
	'cite_error_references_invalid_input' => 'Неправильний тег <code>&lt;references&gt;</code>:
вхідні дані недопустимі. Використовуйте <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'Неправильний тег <code>&lt;references&gt;</code>:
параметри не передбачені. Використовуйте <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters_group' => 'Помилковий тег <code>&lt;references&gt;</code>;
можна використовувати тільки параметр «group».
Використовуйте <code>&lt;references /&gt;</code> або <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => 'Недостатньо символів для зворотних гіперпосилань.
Потрібно розширити системну змінну <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki>',
	'cite_error_references_no_text' => 'Неправильний виклик <code>&lt;ref&gt;</code>:
для виносок <code>$1</code> не вказаний текст',
	'cite_error_included_ref' => 'Відсутній тег &lt;/ref&gt; за наявності тега &lt;ref&gt;',
	'cite_reference_link_key_with_num' => '$1_$2',
	'cite_reference_link_prefix' => 'cite_ref-',
	'cite_references_link_prefix' => 'cite_note-',
	'cite_reference_link' => '<sup id="$1" class="reference">[[#$2|<nowiki>[</nowiki>$3<nowiki>]</nowiki>]]</sup>',
	'cite_references_link_many_format_backlink_labels' => 'а б в г д е ж и к л м н п р с т у ф х ц ш щ ю я аа аб ав аг ад ае аж аи ак ал ам ан ап ар ас ат ау аф ах ац аш ащ аю ая ба бб бв бг бд бе бж би бк бл бм бн бп бр бс бт бу бф бх бц бш бщ бю бя ва вб вв вг вд ве вж ви вк вл вм вн вп вр вс вт ву вф вх вц вш вщ вю вя га гб гв гг гд ге гж ги гк гл гм гн гп гр гс гт гу гф гх гц гш гщ гю гя да дб дв дг дд де дж ди дк дл дм дн дп др дс дт ду дф дх дц дш дщ дю дя еа еб ев ег ед ее еж еи ек ел ем ен еп ер ес ет еу еф ех ец еш ещ ею ея жа жб жв жг жд же жж жи жк жл жм жн жп жр жс жт жу жф жх жц жш жщ жю жя иа иб ив иг ид ие иж ии ик ил им ин ип ир ис ит иу иф их иц иш ищ ию ия ка кб кв кг кд ке кж ки кк кл км кн кп кр кс кт ку кф кх кц кш кщ кю кя ла лб лв лг лд ле лж ли лк лл лм лн лп лр лс лт лу лф лх лц лш лщ лю ля ма мб мв мг мд ме мж ми мк мл мм мн мп мр мс мт му мф мх мц мш мщ мю мя на нб нв нг нд не нж ни нк нл нм нн нп нр нс нт ну нф нх нц нш нщ ню ня па пб пв пг пд пе пж пи пк пл пм пн пп пр пс пт пу пф пх пц пш пщ пю пя ра рб рв рг рд ре рж ри рк рл рм рн рп рр рс рт ру рф рх рц рш рщ рю ря са сб св сг сд се сж си ск сл см сн сп ср сс ст су сф сх сц сш сщ сю ся та тб тв тг тд те тж ти тк тл тм тн тп тр тс тт ту тф тх тц тш тщ тю тя уа уб ув уг уд уе уж уи ук ул ум ун уп ур ус ут уу уф ух уц уш ущ ую уя фа фб фв фг фд фе фж фи фк фл фм фн фп фр фс фт фу фф фх фц фш фщ фю фя ха хб хв хг хд хе хж хи хк хл хм хн хп хр хс хт ху хф хх хц хш хщ хю хя ца цб цв цг цд це цж ци цк цл цм цн цп цр цс цт цу цф цх цц цш цщ цю ця ша шб шв шг шд ше шж ши шк шл шм шн шп шр шс шт шу шф шх шц шш шщ шю шя ща щб щв щг щд ще щж щи щк щл щм щн щп щр щс щт щу щф щх щц щш щщ щю щя юа юб юв юг юд юе юж юи юк юл юм юн юп юр юс ют юу юф юх юц юш ющ юю юя яа яб яв яг яд яе яж яи як ял ям ян яп яр яс ят яу яф ях яц яш ящ яю яя',
);

/** Vèneto (Vèneto)
 * @author Candalua
 */
$messages['vec'] = array(
	'cite_desc' => 'Zonta i tag <nowiki><ref[ name=id]></nowiki> e <nowiki><references/></nowiki> par gestir le citazion',
	'cite_croak' => 'Eror ne la citazion: $1: $2',
	'cite_error_key_str_invalid' => 'Eror interno: $str e/o $key sbaglià. Sta roba qua no la dovarìa mai capitar.',
	'cite_error_stack_invalid_input' => 'Eror interno;
ciave de stack sbaglià.
Sta roba no la dovarìa mai capitar.',
	'cite_error' => 'Eror ne la funsion Cite $1',
	'cite_error_ref_numeric_key' => "Eror ne l'uso del marcator <code>&lt;ref&gt;</code>: el nome no'l pode mìa èssar un nùmaro intiero. Dòpara un titolo esteso",
	'cite_error_ref_no_key' => "Eror ne l'uso del marcator <code>&lt;ref&gt;</code>: i ref vodi no i pol no verghe un nome",
	'cite_error_ref_too_many_keys' => "Eror ne l'uso del marcator <code>&lt;ref&gt;</code>: nomi mìa validi (ad es. nùmaro massa elevà)",
	'cite_error_ref_no_input' => "Eror ne l'uso del marcator <code>&lt;ref&gt;</code>: i ref che no gà un nome no i pol mìa èssar vodi",
	'cite_error_references_invalid_input' => "Eror ne l'uso del marcator <code>&lt;references&gt;</code>: input mìa consentìo, dòpara el marcator
<code>&lt;references /&gt;</code>",
	'cite_error_references_invalid_parameters' => "Eror ne l'uso del marcator <code>&lt;references&gt;</code>: parametri mìa consentìi, dòpara el marcator <code>&lt;references /&gt;</code>",
	'cite_error_references_invalid_parameters_group' => 'Tag <code>&lt;references&gt;</code> mìa valido;
solo el parametro "group" el xe permesso.
Dòpara <code>&lt;references /&gt;</code>, o <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => 'Etichete de rimando personalizàe esaurìe, auménteghen el nùmaro nel messagio <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki>',
	'cite_error_references_no_text' => 'Marcator <code>&lt;ref&gt;</code> mìa valido; no xe stà indicà nissun testo par el marcator <code>$1</code>',
	'cite_error_included_ref' => '&lt;/ref&gt; de chiusura mancante par el marcador &lt;ref&gt;',
);

/** Vietnamese (Tiếng Việt)
 * @author Minh Nguyen
 * @author Vinhtantran
 */
$messages['vi'] = array(
	'cite_desc' => 'Thêm các thẻ <nowiki><ref[ name=id]></nowiki> và <nowiki><references/></nowiki> để ghi chú thích hoặc nguồn tham khảo',
	'cite_croak' => 'Chú thích bị hỏng; $1: $2',
	'cite_error_key_str_invalid' => 'Lỗi nội bộ; $str và/hoặc $key không hợp lệ. Điều này không bao giờ nên xảy ra.',
	'cite_error_stack_invalid_input' => 'Lỗi nội bộ; khóa xác định chồng bị sai.  Đáng ra không bao giờ xảy ra điều này.',
	'cite_error' => 'Lỗi chú thích: $1',
	'cite_error_ref_numeric_key' => 'Thẻ <code>&lt;ref&gt;</code> sai; tên không thể chỉ là số nguyên, hãy dùng tựa đề có tính miêu tả',
	'cite_error_ref_no_key' => 'Thẻ <code>&lt;ref&gt;</code> sai; thẻ ref không có nội dung thì phải có tên',
	'cite_error_ref_too_many_keys' => 'Thẻ <code>&lt;ref&gt;</code> sai; thông số tên sai, như, nhiều thông số tên quá',
	'cite_error_ref_no_input' => 'Mã <code>&lt;ref&gt;</code> sai; thẻ ref không có tên thì phải có nội dung',
	'cite_error_references_invalid_input' => 'Thẻ <code>&lt;references&gt;</code> sai; không được phép nhập nội dung cho thẻ, hãy dùng <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'Thẻ <code>&lt;references&gt;</code> sai; không được có thông số, hãy dùng <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters_group' => 'Thẻ <code>&lt;references&gt;</code> không hợp lệ;
chỉ cho phép tham số "group".
Hãy dùng <code>&lt;references /&gt;</code>, hoặc <code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => 'Đã dùng hết nhãn tham khảo chung.
Hãy định nghĩa thêm ở thông báo <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki>',
	'cite_error_references_no_text' => 'Thẻ <code>&lt;ref&gt;</code> sai; không có nội dung trong thẻ ref có tên <code>$1</code>',
	'cite_error_included_ref' => 'Không có &lt;/ref&gt; để đóng thẻ &lt;ref&gt;',
);

/** Volapük (Volapük)
 * @author Smeira
 */
$messages['vo'] = array(
	'cite_croak' => 'Saitot dädik; $1: $2',
	'cite_error_key_str_invalid' => 'Pöl ninik: $str e/u $key no lonöföl(s). Atos no sötonöv jenön.',
	'cite_error_stack_invalid_input' => 'Pöl ninik; kumakik no lonöföl. Atos neai sötonöv jenön.',
	'cite_error' => 'Saitamapöl: $1',
	'cite_error_ref_numeric_key' => 'Nem ela <code>&lt;ref&gt;</code> no lonöföl. Nem no kanon binädön te me numats; gebolös bepenami.',
	'cite_error_ref_no_key' => 'Geb no lonöföl ela <code>&lt;ref&gt;</code>: els ref nen ninäd mutons labön nemi',
	'cite_error_ref_too_many_keys' => 'El <code>&lt;ref&gt;</code> no lonöfon: labon nemis no lonöfikis, a. s. tumödikis',
	'cite_error_ref_no_input' => 'El <code>&lt;ref&gt;</code> no lonöfon: els ref nen nem mutons labön ninädi',
	'cite_error_references_invalid_input' => 'El <code>&lt;references&gt;</code> no lonöfon; gebolös eli <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'El <code>&lt;references&gt;</code> no lonöfon: paramets no padälons. Gebolös eli <code>&lt;references /&gt;</code>',
	'cite_error_references_no_text' => 'El <code>&lt;ref&gt;</code> no lonöfon: vödem nonik pegivon eles refs labü nem: <code>$1</code>',
);

/** Yiddish (ייִדיש)
 * @author פוילישער
 */
$messages['yi'] = array(
	'cite_desc' => 'לייגט צו <nowiki><ref[ name=id]></nowiki> און <nowiki><references/></nowiki> טאַגן, פֿאר ציטירונגען (אין הערות)',
	'cite_croak' => 'טעות אין ציטירונג; $1: $2',
	'cite_error' => 'ציטירן גרײַז: $1',
	'cite_error_ref_numeric_key' => 'גרײַזיגער <code>&lt;ref&gt;</code> טאַג;
נאמען טאר נישט זײַן קיין פשוטער נומער. ניצט א באשרײַבדיק קעפל',
	'cite_error_ref_no_key' => 'אומגילדיגער <code>&lt;ref&gt;</code> טאַג;
א רעפֿערענץ אָן תוכן מוז האבן א נאמען',
	'cite_error_ref_too_many_keys' => 'אומגילטיגער <code>&lt;ref&gt;</code> טאַג;
אומגילטיגע נעמען, צ.ב. צו פֿיל',
	'cite_error_ref_no_input' => 'אומגילטיגער <code>&lt;ref&gt;</code> טאַג;
א רעפֿערענץ אָן א נאמען דארף האבן תוכן',
	'cite_error_references_invalid_input' => 'אומגילטיגער <code>&lt;references&gt;</code> טאַג;
נעמט נישט קיין תוכן. ניצט <code>&lt;references /&gt;</code>',
	'cite_error_references_invalid_parameters' => 'אומגילטיגער <code>&lt;references&gt;</code> טאַג;
קיין פאראמעטערס נישט ערלויבט. ניצט <code>&lt;references /&gt;</code>',
);

/** Yue (粵語) */
$messages['yue'] = array(
	'cite_desc' => '加 <nowiki><ref[ name=id]></nowiki> 同 <nowiki><references/></nowiki> 標籤用響引用度',
	'cite_croak' => '引用阻塞咗; $1: $2',
	'cite_error_key_str_invalid' => '內部錯誤; 無效嘅 $str',
	'cite_error_stack_invalid_input' => '內部錯誤; 無效嘅堆疊匙',
	'cite_error' => '引用錯誤 $1',
	'cite_error_ref_numeric_key' => '無效嘅呼叫; 需要一個非整數嘅匙',
	'cite_error_ref_no_key' => '無效嘅呼叫; 未指定匙',
	'cite_error_ref_too_many_keys' => '無效嘅呼叫; 無效嘅匙, 例如: 太多或者指定咗一個錯咗嘅匙',
	'cite_error_ref_no_input' => '無效嘅呼叫; 未指定輸入',
	'cite_error_references_invalid_input' => '無效嘅輸入; 唔需要有嘢',
	'cite_error_references_invalid_parameters' => '無效嘅參數; 唔需要有嘢',
	'cite_error_references_invalid_parameters_group' => '無效嘅<code>&lt;references&gt;</code>標籤；
只容許 "group" 參數。
用<code>&lt;references /&gt;</code>，或<code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => '用晒啲自定返回標籤, 響 <nowiki>[[MediaWiki:Cite_references_link_many_format_backlink_labels]]</nowiki> 信息再整多啲',
	'cite_error_references_no_text' => '無效嘅<code>&lt;ref&gt;</code>標籤；
無文字提供於名為<code>$1</code>嘅參照',
);

/** Simplified Chinese (‪中文(简体)‬) */
$messages['zh-hans'] = array(
	'cite_desc' => '加入 <nowiki><ref[ name=id]></nowiki> 与 <nowiki><references/></nowiki> 标签用于引用中',
	'cite_croak' => '引用阻塞; $1: $2',
	'cite_error_key_str_invalid' => '内部错误；非法的 $str',
	'cite_error_stack_invalid_input' => '内部错误；非法堆栈键值',
	'cite_error' => '引用错误 $1',
	'cite_error_ref_numeric_key' => '无效呼叫；需要一个非整数的键值',
	'cite_error_ref_no_key' => '无效呼叫；没有指定键值',
	'cite_error_ref_too_many_keys' => '无效呼叫；非法键值，例如：过多或错误的指定键值',
	'cite_error_ref_no_input' => '无效呼叫；没有指定的输入',
	'cite_error_references_invalid_input' => '无效输入；需求为空',
	'cite_error_references_invalid_parameters' => '非法参数；需求为空',
	'cite_error_references_invalid_parameters_group' => '无效的<code>&lt;references&gt;</code>标签；
只容许 "group" 参数。
用<code>&lt;references /&gt;</code>，或<code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => '自定义后退标签已经用完了，现在可在标签 <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki> 定义更多信息',
	'cite_error_references_no_text' => '无效的<code>&lt;ref&gt;</code>标签；
无文字提供于名为<code>$1</code>的参照',
);

/** Traditional Chinese (‪中文(繁體)‬) */
$messages['zh-hant'] = array(
	'cite_desc' => '加入 <nowiki><ref[ name=id]></nowiki> 與 <nowiki><references/></nowiki> 標籤用於引用中',
	'cite_croak' => '引用阻塞; $1: $2',
	'cite_error_key_str_invalid' => '內部錯誤；非法的 $str',
	'cite_error_stack_invalid_input' => '內部錯誤；非法堆疊鍵值',
	'cite_error' => '引用錯誤 $1',
	'cite_error_ref_numeric_key' => '無效呼叫；需要一個非整數的鍵',
	'cite_error_ref_no_key' => '無效呼叫；沒有指定鍵',
	'cite_error_ref_too_many_keys' => '無效呼叫；非法鍵值，例如：過多或錯誤的指定鍵',
	'cite_error_ref_no_input' => '無效呼叫；沒有指定的輸入',
	'cite_error_references_invalid_input' => '無效輸入；需求為空',
	'cite_error_references_invalid_parameters' => '非法參數；需求為空',
	'cite_error_references_invalid_parameters_group' => '無效的<code>&lt;references&gt;</code>標籤；
只容許 "group" 參數。
用<code>&lt;references /&gt;</code>，或<code>&lt;references group="..." /&gt;</code>',
	'cite_error_references_no_backlink_label' => '自訂後退標籤已經用完了，現在可在標籤 <nowiki>[[MediaWiki:Cite references link many format backlink labels]]</nowiki> 定義更多信息',
	'cite_error_references_no_text' => '無效的<code>&lt;ref&gt;</code>標籤；
無文字提供於名為<code>$1</code>的參照',
);

