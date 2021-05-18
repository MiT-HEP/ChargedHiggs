import os,sys,re
import math
from subprocess import call,check_output
import ROOT

# make sure they are the same and same order of MC::aqgc_names -- python/ParseDat
aqgc_names=["fs0_0p00","fs0_5p00","fs0_10p00","fs0_15p00","fs0_20p00","fs0_25p00","fs0_30p00","fs0_35p00","fs0_40p00","fs0_45p00","fs0_50p00","fs0_55p00","fs0_60p00","fs0_65p00","fs0_70p00","fs0_75p00","fs0_80p00","fs0_85p00","fs0_90p00","fs0_95p00","fs0_100p00","fs0_105p00","fs0_110p00","fs0_115p00","fs0_120p00","fs0_125p00","fs0_130p00","fs0_135p00","fs0_140p00","fs0_145p00","fs0_150p00","fs0_155p00","fs0_160p00","fs0_165p00","fs0_170p00","fs0_175p00","fs0_180p00","fs0_185p00","fs0_190p00","fs0_195p00","fs0_200p00","fs1_m320p00","fs1_m312p00","fs1_m304p00","fs1_m296p00","fs1_m288p00","fs1_m280p00","fs1_m272p00","fs1_m264p00","fs1_m256p00","fs1_m248p00","fs1_m240p00","fs1_m232p00","fs1_m224p00","fs1_m216p00","fs1_m208p00","fs1_m200p00","fs1_m192p00","fs1_m184p00","fs1_m176p00","fs1_m168p00","fs1_m160p00","fs1_m152p00","fs1_m144p00","fs1_m136p00","fs1_m128p00","fs1_m120p00","fs1_m112p00","fs1_m104p00","fs1_m96p00","fs1_m88p00","fs1_m80p00","fs1_m72p00","fs1_m64p00","fs1_m56p00","fs1_m48p00","fs1_m40p00","fs1_m32p00","fs1_m24p00","fs1_m16p00","fs1_m8p00","fs1_0p00","fs1_8p00","fs1_16p00","fs1_24p00","fs1_32p00","fs1_40p00","fs1_48p00","fs1_56p00","fs1_64p00","fs1_72p00","fs1_80p00","fs1_88p00","fs1_96p00","fs1_104p00","fs1_112p00","fs1_120p00","fs1_128p00","fs1_136p00","fs1_144p00","fs1_152p00","fs1_160p00","fs1_168p00","fs1_176p00","fs1_184p00","fs1_192p00","fs1_200p00","fs1_208p00","fs1_216p00","fs1_224p00","fs1_232p00","fs1_240p00","fs1_248p00","fs1_256p00","fs1_264p00","fs1_272p00","fs1_280p00","fs1_288p00","fs1_296p00","fs1_304p00","fs1_312p00","fs1_320p00","fs2_m320p00","fs2_m312p00","fs2_m304p00","fs2_m296p00","fs2_m288p00","fs2_m280p00","fs2_m272p00","fs2_m264p00","fs2_m256p00","fs2_m248p00","fs2_m240p00","fs2_m232p00","fs2_m224p00","fs2_m216p00","fs2_m208p00","fs2_m200p00","fs2_m192p00","fs2_m184p00","fs2_m176p00","fs2_m168p00","fs2_m160p00","fs2_m152p00","fs2_m144p00","fs2_m136p00","fs2_m128p00","fs2_m120p00","fs2_m112p00","fs2_m104p00","fs2_m96p00","fs2_m88p00","fs2_m80p00","fs2_m72p00","fs2_m64p00","fs2_m56p00","fs2_m48p00","fs2_m40p00","fs2_m32p00","fs2_m24p00","fs2_m16p00","fs2_m8p00","fs2_0p00","fs2_8p00","fs2_16p00","fs2_24p00","fs2_32p00","fs2_40p00","fs2_48p00","fs2_56p00","fs2_64p00","fs2_72p00","fs2_80p00","fs2_88p00","fs2_96p00","fs2_104p00","fs2_112p00","fs2_120p00","fs2_128p00","fs2_136p00","fs2_144p00","fs2_152p00","fs2_160p00","fs2_168p00","fs2_176p00","fs2_184p00","fs2_192p00","fs2_200p00","fs2_208p00","fs2_216p00","fs2_224p00","fs2_232p00","fs2_240p00","fs2_248p00","fs2_256p00","fs2_264p00","fs2_272p00","fs2_280p00","fs2_288p00","fs2_296p00","fs2_304p00","fs2_312p00","fs2_320p00","fm0_m36p00","fm0_m35p10","fm0_m34p20","fm0_m33p30","fm0_m32p40","fm0_m31p50","fm0_m30p60","fm0_m29p70","fm0_m28p80","fm0_m27p90","fm0_m27p00","fm0_m26p10","fm0_m25p20","fm0_m24p30","fm0_m23p40","fm0_m22p50","fm0_m21p60","fm0_m20p70","fm0_m19p80","fm0_m18p90","fm0_m18p00","fm0_m17p10","fm0_m16p20","fm0_m15p30","fm0_m14p40","fm0_m13p50","fm0_m12p60","fm0_m11p70","fm0_m10p80","fm0_m9p90","fm0_m9p00","fm0_m8p10","fm0_m7p20","fm0_m6p30","fm0_m5p40","fm0_m4p50","fm0_m3p60","fm0_m2p70","fm0_m1p80","fm0_m0p90","fm0_0p00","fm0_0p90","fm0_1p80","fm0_2p70","fm0_3p60","fm0_4p50","fm0_5p40","fm0_6p30","fm0_7p20","fm0_8p10","fm0_9p00","fm0_9p90","fm0_10p80","fm0_11p70","fm0_12p60","fm0_13p50","fm0_14p40","fm0_15p30","fm0_16p20","fm0_17p10","fm0_18p00","fm0_18p90","fm0_19p80","fm0_20p70","fm0_21p60","fm0_22p50","fm0_23p40","fm0_24p30","fm0_25p20","fm0_26p10","fm0_27p00","fm0_27p90","fm0_28p80","fm0_29p70","fm0_30p60","fm0_31p50","fm0_32p40","fm0_33p30","fm0_34p20","fm0_35p10","fm0_36p00","fm1_m28p00","fm1_m27p30","fm1_m26p60","fm1_m25p90","fm1_m25p20","fm1_m24p50","fm1_m23p80","fm1_m23p10","fm1_m22p40","fm1_m21p70","fm1_m21p00","fm1_m20p30","fm1_m19p60","fm1_m18p90","fm1_m18p20","fm1_m17p50","fm1_m16p80","fm1_m16p10","fm1_m15p40","fm1_m14p70","fm1_m14p00","fm1_m13p30","fm1_m12p60","fm1_m11p90","fm1_m11p20","fm1_m10p50","fm1_m9p80","fm1_m9p10","fm1_m8p40","fm1_m7p70","fm1_m7p00","fm1_m6p30","fm1_m5p60","fm1_m4p90","fm1_m4p20","fm1_m3p50","fm1_m2p80","fm1_m2p10","fm1_m1p40","fm1_m0p70","fm1_0p00","fm1_0p70","fm1_1p40","fm1_2p10","fm1_2p80","fm1_3p50","fm1_4p20","fm1_4p90","fm1_5p60","fm1_6p30","fm1_7p00","fm1_7p70","fm1_8p40","fm1_9p10","fm1_9p80","fm1_10p50","fm1_11p20","fm1_11p90","fm1_12p60","fm1_13p30","fm1_14p00","fm1_14p70","fm1_15p40","fm1_16p10","fm1_16p80","fm1_17p50","fm1_18p20","fm1_18p90","fm1_19p60","fm1_20p30","fm1_21p00","fm1_21p70","fm1_22p40","fm1_23p10","fm1_23p80","fm1_24p50","fm1_25p20","fm1_25p90","fm1_26p60","fm1_27p30","fm1_28p00","fm2_m60p00","fm2_m58p50","fm2_m57p00","fm2_m55p50","fm2_m54p00","fm2_m52p50","fm2_m51p00","fm2_m49p50","fm2_m48p00","fm2_m46p50","fm2_m45p00","fm2_m43p50","fm2_m42p00","fm2_m40p50","fm2_m39p00","fm2_m37p50","fm2_m36p00","fm2_m34p50","fm2_m33p00","fm2_m31p50","fm2_m30p00","fm2_m28p50","fm2_m27p00","fm2_m25p50","fm2_m24p00","fm2_m22p50","fm2_m21p00","fm2_m19p50","fm2_m18p00","fm2_m16p50","fm2_m15p00","fm2_m13p50","fm2_m12p00","fm2_m10p50","fm2_m9p00","fm2_m7p50","fm2_m6p00","fm2_m4p50","fm2_m3p00","fm2_m1p50","fm2_0p00","fm2_1p50","fm2_3p00","fm2_4p50","fm2_6p00","fm2_7p50","fm2_9p00","fm2_10p50","fm2_12p00","fm2_13p50","fm2_15p00","fm2_16p50","fm2_18p00","fm2_19p50","fm2_21p00","fm2_22p50","fm2_24p00","fm2_25p50","fm2_27p00","fm2_28p50","fm2_30p00","fm2_31p50","fm2_33p00","fm2_34p50","fm2_36p00","fm2_37p50","fm2_39p00","fm2_40p50","fm2_42p00","fm2_43p50","fm2_45p00","fm2_46p50","fm2_48p00","fm2_49p50","fm2_51p00","fm2_52p50","fm2_54p00","fm2_55p50","fm2_57p00","fm2_58p50","fm2_60p00","fm3_m80p00","fm3_m78p00","fm3_m76p00","fm3_m74p00","fm3_m72p00","fm3_m70p00","fm3_m68p00","fm3_m66p00","fm3_m64p00","fm3_m62p00","fm3_m60p00","fm3_m58p00","fm3_m56p00","fm3_m54p00","fm3_m52p00","fm3_m50p00","fm3_m48p00","fm3_m46p00","fm3_m44p00","fm3_m42p00","fm3_m40p00","fm3_m38p00","fm3_m36p00","fm3_m34p00","fm3_m32p00","fm3_m30p00","fm3_m28p00","fm3_m26p00","fm3_m24p00","fm3_m22p00","fm3_m20p00","fm3_m18p00","fm3_m16p00","fm3_m14p00","fm3_m12p00","fm3_m10p00","fm3_m8p00","fm3_m6p00","fm3_m4p00","fm3_m2p00","fm3_0p00","fm3_2p00","fm3_4p00","fm3_6p00","fm3_8p00","fm3_10p00","fm3_12p00","fm3_14p00","fm3_16p00","fm3_18p00","fm3_20p00","fm3_22p00","fm3_24p00","fm3_26p00","fm3_28p00","fm3_30p00","fm3_32p00","fm3_34p00","fm3_36p00","fm3_38p00","fm3_40p00","fm3_42p00","fm3_44p00","fm3_46p00","fm3_48p00","fm3_50p00","fm3_52p00","fm3_54p00","fm3_56p00","fm3_58p00","fm3_60p00","fm3_62p00","fm3_64p00","fm3_66p00","fm3_68p00","fm3_70p00","fm3_72p00","fm3_74p00","fm3_76p00","fm3_78p00","fm3_80p00","fm4_m80p00","fm4_m78p00","fm4_m76p00","fm4_m74p00","fm4_m72p00","fm4_m70p00","fm4_m68p00","fm4_m66p00","fm4_m64p00","fm4_m62p00","fm4_m60p00","fm4_m58p00","fm4_m56p00","fm4_m54p00","fm4_m52p00","fm4_m50p00","fm4_m48p00","fm4_m46p00","fm4_m44p00","fm4_m42p00","fm4_m40p00","fm4_m38p00","fm4_m36p00","fm4_m34p00","fm4_m32p00","fm4_m30p00","fm4_m28p00","fm4_m26p00","fm4_m24p00","fm4_m22p00","fm4_m20p00","fm4_m18p00","fm4_m16p00","fm4_m14p00","fm4_m12p00","fm4_m10p00","fm4_m8p00","fm4_m6p00","fm4_m4p00","fm4_m2p00","fm4_0p00","fm4_2p00","fm4_4p00","fm4_6p00","fm4_8p00","fm4_10p00","fm4_12p00","fm4_14p00","fm4_16p00","fm4_18p00","fm4_20p00","fm4_22p00","fm4_24p00","fm4_26p00","fm4_28p00","fm4_30p00","fm4_32p00","fm4_34p00","fm4_36p00","fm4_38p00","fm4_40p00","fm4_42p00","fm4_44p00","fm4_46p00","fm4_48p00","fm4_50p00","fm4_52p00","fm4_54p00","fm4_56p00","fm4_58p00","fm4_60p00","fm4_62p00","fm4_64p00","fm4_66p00","fm4_68p00","fm4_70p00","fm4_72p00","fm4_74p00","fm4_76p00","fm4_78p00","fm4_80p00","fm5_m160p00","fm5_m156p00","fm5_m152p00","fm5_m148p00","fm5_m144p00","fm5_m140p00","fm5_m136p00","fm5_m132p00","fm5_m128p00","fm5_m124p00","fm5_m120p00","fm5_m116p00","fm5_m112p00","fm5_m108p00","fm5_m104p00","fm5_m100p00","fm5_m96p00","fm5_m92p00","fm5_m88p00","fm5_m84p00","fm5_m80p00","fm5_m76p00","fm5_m72p00","fm5_m68p00","fm5_m64p00","fm5_m60p00","fm5_m56p00","fm5_m52p00","fm5_m48p00","fm5_m44p00","fm5_m40p00","fm5_m36p00","fm5_m32p00","fm5_m28p00","fm5_m24p00","fm5_m20p00","fm5_m16p00","fm5_m12p00","fm5_m8p00","fm5_m4p00","fm5_0p00","fm5_4p00","fm5_8p00","fm5_12p00","fm5_16p00","fm5_20p00","fm5_24p00","fm5_28p00","fm5_32p00","fm5_36p00","fm5_40p00","fm5_44p00","fm5_48p00","fm5_52p00","fm5_56p00","fm5_60p00","fm5_64p00","fm5_68p00","fm5_72p00","fm5_76p00","fm5_80p00","fm5_84p00","fm5_88p00","fm5_92p00","fm5_96p00","fm5_100p00","fm5_104p00","fm5_108p00","fm5_112p00","fm5_116p00","fm5_120p00","fm5_124p00","fm5_128p00","fm5_132p00","fm5_136p00","fm5_140p00","fm5_144p00","fm5_148p00","fm5_152p00","fm5_156p00","fm5_160p00","fm7_m80p00","fm7_m78p00","fm7_m76p00","fm7_m74p00","fm7_m72p00","fm7_m70p00","fm7_m68p00","fm7_m66p00","fm7_m64p00","fm7_m62p00","fm7_m60p00","fm7_m58p00","fm7_m56p00","fm7_m54p00","fm7_m52p00","fm7_m50p00","fm7_m48p00","fm7_m46p00","fm7_m44p00","fm7_m42p00","fm7_m40p00","fm7_m38p00","fm7_m36p00","fm7_m34p00","fm7_m32p00","fm7_m30p00","fm7_m28p00","fm7_m26p00","fm7_m24p00","fm7_m22p00","fm7_m20p00","fm7_m18p00","fm7_m16p00","fm7_m14p00","fm7_m12p00","fm7_m10p00","fm7_m8p00","fm7_m6p00","fm7_m4p00","fm7_m2p00","fm7_0p00","fm7_2p00","fm7_4p00","fm7_6p00","fm7_8p00","fm7_10p00","fm7_12p00","fm7_14p00","fm7_16p00","fm7_18p00","fm7_20p00","fm7_22p00","fm7_24p00","fm7_26p00","fm7_28p00","fm7_30p00","fm7_32p00","fm7_34p00","fm7_36p00","fm7_38p00","fm7_40p00","fm7_42p00","fm7_44p00","fm7_46p00","fm7_48p00","fm7_50p00","fm7_52p00","fm7_54p00","fm7_56p00","fm7_58p00","fm7_60p00","fm7_62p00","fm7_64p00","fm7_66p00","fm7_68p00","fm7_70p00","fm7_72p00","fm7_74p00","fm7_76p00","fm7_78p00","fm7_80p00","ft0_m2p00","ft0_m1p95","ft0_m1p90","ft0_m1p85","ft0_m1p80","ft0_m1p75","ft0_m1p70","ft0_m1p65","ft0_m1p60","ft0_m1p55","ft0_m1p50","ft0_m1p45","ft0_m1p40","ft0_m1p35","ft0_m1p30","ft0_m1p25","ft0_m1p20","ft0_m1p15","ft0_m1p10","ft0_m1p05","ft0_m1p00","ft0_m0p95","ft0_m0p90","ft0_m0p85","ft0_m0p80","ft0_m0p75","ft0_m0p70","ft0_m0p65","ft0_m0p60","ft0_m0p55","ft0_m0p50","ft0_m0p45","ft0_m0p40","ft0_m0p35","ft0_m0p30","ft0_m0p25","ft0_m0p20","ft0_m0p15","ft0_m0p10","ft0_m0p05","ft0_0p00","ft0_0p05","ft0_0p10","ft0_0p15","ft0_0p20","ft0_0p25","ft0_0p30","ft0_0p35","ft0_0p40","ft0_0p45","ft0_0p50","ft0_0p55","ft0_0p60","ft0_0p65","ft0_0p70","ft0_0p75","ft0_0p80","ft0_0p85","ft0_0p90","ft0_0p95","ft0_1p00","ft0_1p05","ft0_1p10","ft0_1p15","ft0_1p20","ft0_1p25","ft0_1p30","ft0_1p35","ft0_1p40","ft0_1p45","ft0_1p50","ft0_1p55","ft0_1p60","ft0_1p65","ft0_1p70","ft0_1p75","ft0_1p80","ft0_1p85","ft0_1p90","ft0_1p95","ft0_2p00","ft1_m2p00","ft1_m1p95","ft1_m1p90","ft1_m1p85","ft1_m1p80","ft1_m1p75","ft1_m1p70","ft1_m1p65","ft1_m1p60","ft1_m1p55","ft1_m1p50","ft1_m1p45","ft1_m1p40","ft1_m1p35","ft1_m1p30","ft1_m1p25","ft1_m1p20","ft1_m1p15","ft1_m1p10","ft1_m1p05","ft1_m1p00","ft1_m0p95","ft1_m0p90","ft1_m0p85","ft1_m0p80","ft1_m0p75","ft1_m0p70","ft1_m0p65","ft1_m0p60","ft1_m0p55","ft1_m0p50","ft1_m0p45","ft1_m0p40","ft1_m0p35","ft1_m0p30","ft1_m0p25","ft1_m0p20","ft1_m0p15","ft1_m0p10","ft1_m0p05","ft1_0p00","ft1_0p05","ft1_0p10","ft1_0p15","ft1_0p20","ft1_0p25","ft1_0p30","ft1_0p35","ft1_0p40","ft1_0p45","ft1_0p50","ft1_0p55","ft1_0p60","ft1_0p65","ft1_0p70","ft1_0p75","ft1_0p80","ft1_0p85","ft1_0p90","ft1_0p95","ft1_1p00","ft1_1p05","ft1_1p10","ft1_1p15","ft1_1p20","ft1_1p25","ft1_1p30","ft1_1p35","ft1_1p40","ft1_1p45","ft1_1p50","ft1_1p55","ft1_1p60","ft1_1p65","ft1_1p70","ft1_1p75","ft1_1p80","ft1_1p85","ft1_1p90","ft1_1p95","ft1_2p00","ft2_m4p00","ft2_m3p90","ft2_m3p80","ft2_m3p70","ft2_m3p60","ft2_m3p50","ft2_m3p40","ft2_m3p30","ft2_m3p20","ft2_m3p10","ft2_m3p00","ft2_m2p90","ft2_m2p80","ft2_m2p70","ft2_m2p60","ft2_m2p50","ft2_m2p40","ft2_m2p30","ft2_m2p20","ft2_m2p10","ft2_m2p00","ft2_m1p90","ft2_m1p80","ft2_m1p70","ft2_m1p60","ft2_m1p50","ft2_m1p40","ft2_m1p30","ft2_m1p20","ft2_m1p10","ft2_m1p00","ft2_m0p90","ft2_m0p80","ft2_m0p70","ft2_m0p60","ft2_m0p50","ft2_m0p40","ft2_m0p30","ft2_m0p20","ft2_m0p10","ft2_0p00","ft2_0p10","ft2_0p20","ft2_0p30","ft2_0p40","ft2_0p50","ft2_0p60","ft2_0p70","ft2_0p80","ft2_0p90","ft2_1p00","ft2_1p10","ft2_1p20","ft2_1p30","ft2_1p40","ft2_1p50","ft2_1p60","ft2_1p70","ft2_1p80","ft2_1p90","ft2_2p00","ft2_2p10","ft2_2p20","ft2_2p30","ft2_2p40","ft2_2p50","ft2_2p60","ft2_2p70","ft2_2p80","ft2_2p90","ft2_3p00","ft2_3p10","ft2_3p20","ft2_3p30","ft2_3p40","ft2_3p50","ft2_3p60","ft2_3p70","ft2_3p80","ft2_3p90","ft2_4p00","ft5_m8p00","ft5_m7p80","ft5_m7p60","ft5_m7p40","ft5_m7p20","ft5_m7p00","ft5_m6p80","ft5_m6p60","ft5_m6p40","ft5_m6p20","ft5_m6p00","ft5_m5p80","ft5_m5p60","ft5_m5p40","ft5_m5p20","ft5_m5p00","ft5_m4p80","ft5_m4p60","ft5_m4p40","ft5_m4p20","ft5_m4p00","ft5_m3p80","ft5_m3p60","ft5_m3p40","ft5_m3p20","ft5_m3p00","ft5_m2p80","ft5_m2p60","ft5_m2p40","ft5_m2p20","ft5_m2p00","ft5_m1p80","ft5_m1p60","ft5_m1p40","ft5_m1p20","ft5_m1p00","ft5_m0p80","ft5_m0p60","ft5_m0p40","ft5_m0p20","ft5_0p00","ft5_0p20","ft5_0p40","ft5_0p60","ft5_0p80","ft5_1p00","ft5_1p20","ft5_1p40","ft5_1p60","ft5_1p80","ft5_2p00","ft5_2p20","ft5_2p40","ft5_2p60","ft5_2p80","ft5_3p00","ft5_3p20","ft5_3p40","ft5_3p60","ft5_3p80","ft5_4p00","ft5_4p20","ft5_4p40","ft5_4p60","ft5_4p80","ft5_5p00","ft5_5p20","ft5_5p40","ft5_5p60","ft5_5p80","ft5_6p00","ft5_6p20","ft5_6p40","ft5_6p60","ft5_6p80","ft5_7p00","ft5_7p20","ft5_7p40","ft5_7p60","ft5_7p80","ft5_8p00","ft6_m8p00","ft6_m7p80","ft6_m7p60","ft6_m7p40","ft6_m7p20","ft6_m7p00","ft6_m6p80","ft6_m6p60","ft6_m6p40","ft6_m6p20","ft6_m6p00","ft6_m5p80","ft6_m5p60","ft6_m5p40","ft6_m5p20","ft6_m5p00","ft6_m4p80","ft6_m4p60","ft6_m4p40","ft6_m4p20","ft6_m4p00","ft6_m3p80","ft6_m3p60","ft6_m3p40","ft6_m3p20","ft6_m3p00","ft6_m2p80","ft6_m2p60","ft6_m2p40","ft6_m2p20","ft6_m2p00","ft6_m1p80","ft6_m1p60","ft6_m1p40","ft6_m1p20","ft6_m1p00","ft6_m0p80","ft6_m0p60","ft6_m0p40","ft6_m0p20","ft6_0p00","ft6_0p20","ft6_0p40","ft6_0p60","ft6_0p80","ft6_1p00","ft6_1p20","ft6_1p40","ft6_1p60","ft6_1p80","ft6_2p00","ft6_2p20","ft6_2p40","ft6_2p60","ft6_2p80","ft6_3p00","ft6_3p20","ft6_3p40","ft6_3p60","ft6_3p80","ft6_4p00","ft6_4p20","ft6_4p40","ft6_4p60","ft6_4p80","ft6_5p00","ft6_5p20","ft6_5p40","ft6_5p60","ft6_5p80","ft6_6p00","ft6_6p20","ft6_6p40","ft6_6p60","ft6_6p80","ft6_7p00","ft6_7p20","ft6_7p40","ft6_7p60","ft6_7p80","ft6_8p00","ft7_m16p00","ft7_m15p60","ft7_m15p20","ft7_m14p80","ft7_m14p40","ft7_m14p00","ft7_m13p60","ft7_m13p20","ft7_m12p80","ft7_m12p40","ft7_m12p00","ft7_m11p60","ft7_m11p20","ft7_m10p80","ft7_m10p40","ft7_m10p00","ft7_m9p60","ft7_m9p20","ft7_m8p80","ft7_m8p40","ft7_m8p00","ft7_m7p60","ft7_m7p20","ft7_m6p80","ft7_m6p40","ft7_m6p00","ft7_m5p60","ft7_m5p20","ft7_m4p80","ft7_m4p40","ft7_m4p00","ft7_m3p60","ft7_m3p20","ft7_m2p80","ft7_m2p40","ft7_m2p00","ft7_m1p60","ft7_m1p20","ft7_m0p80","ft7_m0p40","ft7_0p00","ft7_0p40","ft7_0p80","ft7_1p20","ft7_1p60","ft7_2p00","ft7_2p40","ft7_2p80","ft7_3p20","ft7_3p60","ft7_4p00","ft7_4p40","ft7_4p80","ft7_5p20","ft7_5p60","ft7_6p00","ft7_6p40","ft7_6p80","ft7_7p20","ft7_7p60","ft7_8p00","ft7_8p40","ft7_8p80","ft7_9p20","ft7_9p60","ft7_10p00","ft7_10p40","ft7_10p80","ft7_11p20","ft7_11p60","ft7_12p00","ft7_12p40","ft7_12p80","ft7_13p20","ft7_13p60","ft7_14p00","ft7_14p40","ft7_14p80","ft7_15p20","ft7_15p60","ft7_16p00","ft8_m20p00","ft8_m19p50","ft8_m19p00","ft8_m18p50","ft8_m18p00","ft8_m17p50","ft8_m17p00","ft8_m16p50","ft8_m16p00","ft8_m15p50","ft8_m15p00","ft8_m14p50","ft8_m14p00","ft8_m13p50","ft8_m13p00","ft8_m12p50","ft8_m12p00","ft8_m11p50","ft8_m11p00","ft8_m10p50","ft8_m10p00","ft8_m9p50","ft8_m9p00","ft8_m8p50","ft8_m8p00","ft8_m7p50","ft8_m7p00","ft8_m6p50","ft8_m6p00","ft8_m5p50","ft8_m5p00","ft8_m4p50","ft8_m4p00","ft8_m3p50","ft8_m3p00","ft8_m2p50","ft8_m2p00","ft8_m1p50","ft8_m1p00","ft8_m0p50","ft8_0p00","ft8_0p50","ft8_1p00","ft8_1p50","ft8_2p00","ft8_2p50","ft8_3p00","ft8_3p50","ft8_4p00","ft8_4p50","ft8_5p00","ft8_5p50","ft8_6p00","ft8_6p50","ft8_7p00","ft8_7p50","ft8_8p00","ft8_8p50","ft8_9p00","ft8_9p50","ft8_10p00","ft8_10p50","ft8_11p00","ft8_11p50","ft8_12p00","ft8_12p50","ft8_13p00","ft8_13p50","ft8_14p00","ft8_14p50","ft8_15p00","ft8_15p50","ft8_16p00","ft8_16p50","ft8_17p00","ft8_17p50","ft8_18p00","ft8_18p50","ft8_19p00","ft8_19p50","ft8_20p00","ft9_m20p00","ft9_m19p50","ft9_m19p00","ft9_m18p50","ft9_m18p00","ft9_m17p50","ft9_m17p00","ft9_m16p50","ft9_m16p00","ft9_m15p50","ft9_m15p00","ft9_m14p50","ft9_m14p00","ft9_m13p50","ft9_m13p00","ft9_m12p50","ft9_m12p00","ft9_m11p50","ft9_m11p00","ft9_m10p50","ft9_m10p00","ft9_m9p50","ft9_m9p00","ft9_m8p50","ft9_m8p00","ft9_m7p50","ft9_m7p00","ft9_m6p50","ft9_m6p00","ft9_m5p50","ft9_m5p00","ft9_m4p50","ft9_m4p00","ft9_m3p50","ft9_m3p00","ft9_m2p50","ft9_m2p00","ft9_m1p50","ft9_m1p00","ft9_m0p50","ft9_0p00","ft9_0p50","ft9_1p00","ft9_1p50","ft9_2p00","ft9_2p50"]


def FindBasePath(verbose=False):
     if verbose: print "-> Looking for basepath"
     basepath = ""
     mypath = os.path.abspath(os.getcwd())
     while mypath != "" and mypath != "/":
             if "ChargedHiggs" in os.path.basename(mypath):
                     basepath = os.path.abspath(mypath)
             mypath = os.path.dirname(mypath)

     if verbose: print "-> Base Path is " + basepath
     sys.path.insert(0,basepath)
     sys.path.insert(0,"./")
     return basepath

def Default():
	config = {}
	config['config'] = {}
	config['Loader'] = "LoadNero"
	return config;


def BoolKey(value):
	value = value.lower()
	if 'no' in value: return False
	if 'yes' in value: return True 
	if 'false' in value : return False
	if 'true' in value : return True
	if value =='n' : return False
	if value =='y': return True
	return False

## TODO, make a class for the parser
subdict={}

def StringKey(value):
	try:
		if '%' in value: value = value%subdict
	except ValueError:
		print "Unable to substitute in value",value,"with dict",subdict
		raise ValueError
	value = re.sub('\n','',value)
	return value

def FloatKey(value):
	return float(value)

def IntKey(value):
	return int(value)

def vStringKey(value):
	r=[]
	for s in value.split(','):
		r.append( StringKey(s) )
	return r

def vFloatKey(value):
	s = vStringKey(value)
	r = [ FloatKey(v) for v in s if v!= '' ]
	return r

def vIntKey(value):
	s = vStringKey(value)
	r = [ IntKey(v) for v in s if v!= '' ]
	return r

def ParseDat(name):
	''' Parse configuratino File '''
	print "<-> Parsing input file", name

	try:
		stream = open(name)
	except IOError:
		base= FindBasePath() ## make sure to be able to open file correctly
		stream = open( base +"/" + name) 

	config = Default()
	for line in stream:
		l = line.split('#')[0]
		l = re.sub(' ' ,'', l )
		if len(l.split()) == 0 : continue;
		l.split()[0]
		if  l =='': continue;

		key = l.split('=')[0]
		value = ''
		if '=' in l : value = '='.join(l.split('=')[1:])
		value=re.sub('~',' ',value)
		######### BOOL  ###########
		if key == "Dump" or key=="Final" or key == "xrdcp":
			config[key]=BoolKey(value)
		######### SUB ######
		if key == 'sub':
			## make sure that is well formatted
			if value.split('|')[0] in subdict and subdict[value.split('|')[0] ] != StringKey(value.split('|')[1]):
				print "WARNING: overriding substitution:",value.split('|')[0],"-> '" + StringKey(value.split('|')[1]) + "' (old value was '"+ subdict[value.split('|')[0] ]+"')" 
			subdict[value.split('|')[0] ]= StringKey( value.split('|')[1] )
		######### STRING ###########
		if key == 'MCDB' or key =='SFDB' or key =='Output' or key == 'pileup' or key =='DumpDir' or key == 'Loader':
			config[key] = StringKey(value)

		####### V STRING ##########
		if      key=='Files' \
			or key == 'Analysis' \
			or key == 'Smear'  \
			or key == 'Correct'  \
			or key == 'Branches': 
			config[key] =  vStringKey(  value   )
	
		if key=='addfiles':
			if 'Files' not in config: config['Files']=[]
			config['Files'].extend(vStringKey( value ))
		####### FLOAT ##########
		if key == 'Lumi':
			config[key]=FloatKey(value)
		####### INT ##########
		if key == 'EntryPerSecond':
			config[key]=IntKey(value)
		####### V FLOAT ##########
		if key == 'pileupLumi':
			config[key]=vFloatKey(value)

		####### V INT ##########
		if key == 'pileupRun':
			config[key]=vIntKey(value)

		########## DICT ###########
		if key == 'config': # dict
			type= value.split('|')[0]
			config[key][type] = vStringKey(  '|'.join(value.split('|')[1:])  )  
		if key == 'addConfig': # dict
			key = 'config'
			type= value.split('|')[0]
			config[key][type].extend(vStringKey(  '|'.join(value.split('|')[1:])  )  )
		
		######## INCLUDE ###########
		if key == 'include':
			R = ParseDat( StringKey(value) )
			for key in R:
				if key == 'config':
					for type in R[key]:
						config[key][type] = R[key][type]
				elif key == 'Files' and 'Files' in config: ## if already exists, extend it, otherwise cgo to default (sub)
					config['Files'].extend( R['Files'] )
				elif key == 'Loader' : # LoadNero is the default and all the dat have it. FIXME
					if R[key] != "LoadNero": config[key] = R[key]
				else:
					config[key] = R[key]
	return config

def FindHadoop(name,mount="/mnt/hadoop/cms"):
	if os.path.isfile(name): return [name] ## file exists
	cmd = "find %s/%s -type f "%(mount,name)
	list=check_output(cmd, shell=True).split()
	removed = [f for f in list if '/failed/' in f ]
	for f in removed:
		print "ParseDat.py - FindHadoop: Ignoring failed file: '"+ f + "'"
	list = [ f for f in list if '/failed/' not in f ]
	return list

def FindEOS(name,mount=""):
	''' EOS PATH should be followed. The mount option will assume that eos is mounted in ~/eos '''
	#EOS = "/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select"
	#EOS = "/afs/cern.ch/project/eos/installation/0.3.15/bin/eos.select"
	EOS = "/usr/bin/eos"
	# should match wildcard, that for some reason new root does not
	if '/store/' not in name and '/eos/user' not in name: return [name]
	if '/eos/cms/store/' in name: return [name] # likely already parsed
	if 'root://eoscms//' in name: return [name] # already parsed
	if 'root://eosuser//' in name: return [name] # already parsed
	if 'root://' in name: return [name] # already parsed
	if os.path.isfile(name): return [name] ## file exists
	
	userInstance=""
	if '/eos/user' in name:
		userInstance=" root://eosuser"
	cmd = EOS + userInstance+ ' find -f ' + name
	print "Runnig command:",cmd
	list = check_output(cmd ,shell=True).split()
	
	# print removed
	tot = len(list)
	removed = [ f for f in list if '/failed/' in f ] 
	for f in removed:
		print "ParseDat.py - FindEOS: Ignoring failed file: '"+ f + "'"
	# remove failed directories from crab submission
	list = [ f  for f in list if '/failed/' not in f ]

	if mount != "":
		if userInstance != "": 
			print "UNSUPPORTED: mount + instance"
			raise ValueError
		fileList = [ re.sub("/eos/cms", mount + "/cms",f) for f in list ]
	else:
		if userInstance =="":
			fileList = [ re.sub("/eos/cms","root://eoscms//",f) for f in list ]
		else:
			fileList = [ re.sub("/eos/user","root://eosuser///eos/user",f) for f in list ]

	return fileList

def FindDataset(name,mount=""): 
    DASclient = "dasgoclient -query '%(query)s'"
    cmd= DASclient%{'query':'file dataset=%s'%name}
    if mount =="":
        o=check_output(cmd,shell=True).split()
        fileList=[ 'root://eoscms//'+x for x in o ]
        if fileList == []: return
        ## try to open file0 -> Dynamically moving to AAA
        f0= ROOT.TFile.Open(fileList[0])
        if f0== None: fileList=[ 'root://xrootd-cms.infn.it//'+x for x in o]
    elif mount =="full-check":
        o=check_output(cmd,shell=True).split()
        fileList=[ 'root://eoscms//'+x for x in o ]
        if fileList == []: return
        ## try to open all files -> Dynamically moving to AAA
        for i in range(0,len(o)):
            f0= ROOT.TFile.Open(fileList[i])
            if f0== None: fileList[i]='root://xrootd-cms.infn.it//'+o[i]
    elif mount == "aaa" or mount == "AAA":
        fileList=[ 'root://xrootd-cms.infn.it//'+x for x in check_output(cmd,shell=True).split() ]
    else:
        fileList=[ mount+x for x in check_output(cmd,shell=True).split() ]
    return fileList

def PrintDat(config):
	''' Print configuration'''

	print "##########################"
	for key in config:
		if key == 'config':
			for type in config[key]:
				print key,'=',type,'|',",".join(config[key][type])
		######### STRING ###########
		elif key == 'MCDB' or key =='SFDB' or key == 'Output' or key == 'pileup':
			print key ,'=',config[key]
		######### V STRING ###########
		elif key =='Files' \
			or key == 'Analysis' \
			or key == 'Smear' \
			or key == 'Correct' \
			or key == 'Branches' :
			print key, '=', ','.join(config[key])
		######### V FLOAT/INT #########
		elif key == 'pileupLumi' \
			or key == 'pileupRun':
			tmp= [ str(n) for n in config[key] ]
			print key, '=', ','.join(tmp)

		else:
			print key , '=' , config[key]
	print "##########################"

def PrintUsage():
	print '###Configuration file###'
	print 'include = filename.dat'
	print 'Loader = LoadNero'
	print 'Files = file1,file2,file3'
	print 'addfiles = file4'
	print 'Output = file'
	print 'MCDB = file'
	print 'SFDB = file'
	print 'pileup = file'
	print 'pileupRun  = ""/ -1 / 1,10,100,100'
	print 'pileupLumi = ""/ -1 /  1.2,2.3,4.5 # partial luminosity '
	print 'branches = brancfile'
	print 'Analysis = AnalysisBase,Analysis2 ..'
	print 'Smears = @SmearBase,JER,JES'
	print 'Correct = @CorrectorBase'
	print 'config = AnalysisBase|a=1,b=2,c(3)'

def ReadMCDB(file):
    '''read and parse the MCDB file:
        \t\t### LABEL dir Entries xSec
    '''
    R={}
    f = open(file)
    for line in f:
        l = line.split('#')[0]
        l = re.sub('\n','',l)
        if l == "": continue
        l=re.sub('^ *','',l)
        label= l.split(' ')[0]
        dir= l.split(' ')[1]
        entries = l.split(' ')[2]
        xsec = l.split(' ')[3]
        scales=[]
        pdfs=[]
        aqgc=[]
        current=4
        while len(l.split(' ') )>current:
            what=l.split(' ')[current]
            current+=1
            if 'PDFS' in what :
                for i in range(0,100):## MC_MAX_PDFS
                    pdfs.append(float(l.split(' ')[current]))
                    current+=1
            elif 'SCALES' in what:
                for i in range(0,6): ## MC_MAX_SCALESS
                    scales.append(float(l.split(' ')[current]))
                    current+=1
            elif 'AQGC' in what:
                for i in range(0,len(aqgc_names)): ## MC_MAX_SCALESS
                    aqgc.append(float(l.split(' ')[current]))
                    current+=1
            else:
             print "WARNING","ignoring field '"+what+"' in MCDB line"
          
        if label in R :
            print "WARNING: Duplicate label",label
        R[label] = (dir,entries,xsec,scales,pdfs,aqgc)
    return R

import json
def ReadSFDB(file,verbose=False):
	'''read and parse the SFDB file:
	    \t\t### LABEL type -- -- --- --- 
	'''
	L=[]## this is a list of sf bins
	f = open(file)
	for line in f:
	   try:
		l = line.split('#')[0]
		l = l.replace('\n','') ## delete end of line
		if l == "": continue
		l=re.sub('^ *','',l) ## remove space at the beginning
		l=re.sub('\ +',' ',l) ## squeeze
		if l == "": continue
		label= l.split(' ')[0]
		type= l.split(' ')[1]
		R={}
		R['label']= label
		R['type'] = type

		if label.lower() == 'include':
			if verbose: print "-> Reading SF from file",type ### DEBUG
			tmp = ReadSFDB(type)
			#remove from L all the key with the same label as in tmp
			labels = set([])
			new = []

			for key in tmp:
				## DEBUG
				if verbose: print " * found key:",key ### DEBUG
				labels.add( key['label'])
				new.append(key)

			for key in L:
				if key['label'] not in labels:
					new.append( key ) 
					if verbose: print " * copying key :",key ### DEBUG
				else:
					if verbose: print " * overwriting key :",key ### DEBUG
			# merge L and tmp
			L = new[:]

		elif type == 'pteta':
			pt1  = float ( l.split(' ')[2] )
			pt2  = float ( l.split(' ')[3] )
			eta1 = float ( l.split(' ')[4] )
			eta2 = float ( l.split(' ')[5] )
			sf   = float ( l.split(' ')[6] )
			err  = float ( l.split(' ')[7] )
			R['pt1']=pt1
			R['pt2']=pt2
			R['eta1']=eta1
			R['eta2']=eta2
		elif type == 'spline':
			pt = float( l.split(' ')[2])
			sf   = float ( l.split(' ')[3] )
			err  = float ( l.split(' ')[4] )
			R['pt']=pt

		elif type == 'th2f':
			R['filename'] = l.split(' ' )[2]
			if len(l.split(' ')) >= 5:
				R['type'] ='th2eff'
				type = 'th2eff'
				R['effdata'] = l.split(' ')[3]
				R['effmc'] = l.split(' ')[4]
				R['errdata'] = ""
				R['errmc'] = ""
				if len(l.split(' ')) >= 7:
					R['errdata'] = l.split(' ')[5]
					R['errmc'] = l.split(' ')[6]
			sf=0.0 ## ignored
			err=0.0 ## ignored

		elif type == 'base':
			sf  = float ( l.split(' ') [2] )
			err = float ( l.split(' ') [3] )

		elif type == 'wg1':
			sf  = 1.0
			err = 0.0

		elif type == 'nnlops':
			sf  = 1.0
			err = 0.0
			R['filename'] = l.split(' ')[2]

		elif type == 'csv':
			R['filename'] = l.split(' ')[2]
			sf=0.0 ## ignored
			err=0.0 ## ignored

		elif type == 'csv-reweight':
			#R['csv-reweight'] 
			sf=0.0 ## ignored
			err=0.0 ## ignored

		elif type == 'tf1' or type=='tf2':
			R['formula'] = l.split(' ' )[2]
			try: R['errFormula'] = l.split(' ')[3]
			except: R['errFormula']=''
			sf=0.0 ## ignored
			err=0.0 ## ignored

		elif type == 'inverse-eff-xinmei':
			R['filename'] = l.split(' ')[2]
			R['type'] = 'pteta'
			fIn = ROOT.TFile.Open(R['filename'])
			hEff= fIn.Get("hEffEtaPt")
			hErr= fIn.Get("hErrhEtaPt") ### high and symm
			for xBin in range(1,hEff.GetXaxis().GetNbins()+1):
			   for yBin in range(1,hEff.GetYaxis().GetNbins()+1):
			      R['pt1'] = hEff.GetYaxis().GetBinLowEdge(yBin)
			      R['pt2'] = hEff.GetYaxis().GetBinLowEdge(yBin+1)
			      R['eta1'] = hEff.GetXaxis().GetBinLowEdge(xBin)
			      R['eta2'] = hEff.GetXaxis().GetBinLowEdge(xBin+1)
			      R['sf'] = 1./hEff.GetBinContent(xBin,yBin)
			      R['err'] = hErr.GetBinContent(xBin,yBin)/(hEff.GetBinContent(xBin,yBin)**2)

			      R1={} ## copy 
			      for key in R: R1[key] = R[key]
			      L.append(R1)
			R=None
			continue ## lines loop

		elif type == 'json-sami':
			R['filename'] = l.split(' ')[2]
			R['type'] = 'pteta'

			doEffOnly=False
			if 'effonly' in ' '.join(l.split()[3:]): 
				doEffOnly=True
				if verbose: print "  * EffOnly. MC eff=1"
			try: 
			   jstring = open(R['filename']).read()
			except IOError as e:
			   print "-> Error in reading SF",R['filename']
			   raise e
		        j=json.loads( jstring )
			"dataParameters"
			"mcParameters"
			#run="runs_256629_260627"
			run="runs_271036_279588"
			runMC="2016"
			for idx in range(3,len(l.split())):
				if 'run=' in l.split()[idx]: run=l.split()[idx].replace('run=','')
				if 'runMC=' in l.split()[idx]: runMC=l.split()[idx].replace('runMC=','')
			for idx,params in enumerate(j["dataParameters"][run]["bins"]):
				R["pt1"] =params["pt"] 
				if len(j["dataParameters"][run]["bins"]) > idx+1:
					next=j["dataParameters"][run]["bins"][idx+1]
					R["pt2"] = next["pt"]
				else:
					R["pt2"] = 8000.
				R["eta1"] =  -10
				R["eta2"] = 10
				effD = params["efficiency"]
				## TODO: implement Asymm
				errD = params["uncertaintyPlus"]
				mc=j["mcParameters"][runMC]["bins"]
				effMc= mc[idx]["efficiency"]
				errMc = mc[idx]["uncertaintyPlus"] 

				if effMc == 0. : continue # bin not well populated

				if doEffOnly:
					effMc=1
					errMc=0

				try:
					R["sf"] = effD/effMc
					R["err"] = math.sqrt((errD/effD)**2 + (errMc/effMc)**2)* R["sf"]
				except ZeroDivisionError:
					R["sf"] =1.0
					R["err"] =0.0

				print "  * Loading SF: ",R["pt1"],R["pt2"]," -- ",R["sf"],"+/-",R["err"]
				R1={} ## copy 
				for key in R: R1[key] = R[key]
				L.append(R1)
			R=None
			continue ## lines loop

		elif type == 'json-muon':
			#usage json-muon aux/mu/RunBC_data_ID.json aux/mu/RunBC_mc_ID.json NUM_SoftID_DEN_genTracks
			# assuming same name in data and mc
			R['filename-data'] = l.split(' ')[2]
			R['filename-mc'] = l.split(' ')[3]
			R['id-type'] = l.split(' ')[4] 
			R['type'] = 'ptetaeff'
			jstring = open(R['filename-data']).read()
			jData=json.loads( jstring )
			jstring = open(R['filename-mc']).read()
			jMc = json.loads(jstring)
			print "* Reading SF from",R['filename-data'],R["filename-mc"],R['id-type']
			if R['id-type'] not in jData:
				print "[ERROR] unknown id",R['id-type'], "in",R['filename-data']
				print "available id are:", ','.join([k for k in jData])
				raise ValueError
			if R['id-type'] not in jMc:
				print "[ERROR] unknown id",R['id-type'], "in",R['filename-mc']
				print "available id are:", ','.join([k for k in jMc])
				raise ValueError
			idData = jData[R['id-type']]
			idMc = jMc[R['id-type']]
			if 'abseta_pt' not in idData:
				print "[ERROR]",'abseta_pt',"not in json"
				print "available sf are", ','.join([k for k in idData]),"in",R["filename-data"]
				raise ValueError
			if 'abseta_pt' not in idMc:
				print "[ERROR]",'abseta_pt',"not in json"
				print "available sf are", ','.join([k for k in idMc]),"in",R["filename-mc"]
				raise ValueError

			for etarange in idData['abseta_pt']:
				if etarange not in idMc['abseta_pt']:
					print "[ERROR] eta range",etarange,"not common between data and mc files"
					raise ValueError
				etaStr= re.sub("abseta:\\[","", re.sub('\\]','',etarange))
				R['eta1'] = float(etaStr.split(',')[0])
				R['eta2'] = float(etaStr.split(',')[1])
				for ptrange in idData['abseta_pt'][etarange]:
					if ptrange not in idMc['abseta_pt'][etarange]:
					    print "[ERROR] pt range",ptrange,"not common between data and mc files in eta range",etarange
					    raise ValueError
					ptStr = re.sub('pt:\\[','',re.sub('\\]','',ptrange))
					R['pt1'] = float(ptStr.split(',')[0])
					R['pt2'] = float(ptStr.split(',')[1])

					dataEff = idData['abseta_pt'][etarange][ptrange]['value']
					dataErr = idData['abseta_pt'][etarange][ptrange]['error']
					mcEff = idMc['abseta_pt'][etarange][ptrange]['value']
					mcErr = idMc['abseta_pt'][etarange][ptrange]['error']
                    
					R["dataEff"]=dataEff
					R["dataErr"]=dataErr
					R["mcEff"]=mcEff
					R["mcErr"]=mcErr

					print "  * Loading Effiencies: ",R["pt1"],R["pt2"],R['eta1'],R['eta2']," -- ",R["dataEff"],R["mcEff"]
					R1={} ## copy 
					for key in R: R1[key] = R[key]
					L.append(R1)
			R=None
			continue ## lines loop
			 
		else:
			print "sf",label,"of type",type,"not supported in the sf database"
		R['sf'] =sf
		R['err'] =err
		L.append(R);

	   except:
		   print "Unable to parse line:"
		   print "'" + line.replace('\n','') + "'"
		   raise ## re-raise exception
	return L

def ReadBranches(fileName):
    ''' Read Branches from a files '''
    R=[]
    if fileName =="": 
        print "[WARNING]: Branch file not provided. Returning empty list."
        return R
    f =open(fileName,"r")
    for line in f:
        l = line.split('#')[0]
        l = re.sub('\n','',l)
        if l == "": continue
        l=re.sub('^ *','',l).split()[0]
        b = StringKey(l)
        R.append(b)
    return R

def chunkIt(seq, num):
  ''' Take a list seq and return a n lists'''
  R=[]
  for i in range(0,num): R.append([])
  for j in range(0,len(seq) ):
        R[ j%num ].append(seq[j])
  return R


from datetime import datetime
class DirectoryStore:
    ''' This class aims to identify which directories are used and by whom. Using datetime, json, os'''
    def __init__(self):
        self.fname="./.nero.json"
        self.isinit=False
        self.db={}
        self.verbose=False

    def init(self):
        ''' Read the database '''
        self.db ={} 
        self.user = os.environ['USER']
        if self.verbose: print "[DEBUG]: User is",self.user
        self.isinit=True

        if self.verbose: print "[DEBUG]: Reading File:",self.fname
        try: 
            jstring = open(self.fname).read()
        except IOError as e:
            print "[ERROR] Unable to open file",self.fname,"assuming it doesn't exist"
            return self

        self.db=json.loads( jstring )
        return self

    def add(self,d):
        ''' Add or update directory usage '''
        if d not in self.db:
            self.db[d]={}
        #if self.user not in self.db[d]:self.db[d][self.user] =""
        ## assuming that now is always an update
        self.db[d][self.user] = "%d/%d/%d"%(datetime.today().year,datetime.today().month,datetime.today().day)
        return self
    def end(self):
        ''' write the database'''
        jstring = json.dumps(self.db,sort_keys=True, indent=4)
        fd=open(self.fname,"write")
        fd.write(jstring)
        fd.close()
        self.isinit=False
        return self

