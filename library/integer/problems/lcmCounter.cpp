#include <cmath>
#include <vector>
#include <algorithm>

using namespace std;

#include "lcmCounter.h"

/////////// For Testing ///////////////////////////////////////////////////////

#include <time.h>
#include <cassert>
#include <iostream>
#include "../../common/iostreamhelper.h"
#include "../../common/profile.h"
#include "../../common/rand.h"

const long long MAXR = 10'000'000'000'000'000ll;

// { (X, LCM count), ... }
static pair<long long, int> sLcmCount[]{
    { 6ll, 2 },
    { 12ll, 3 },
    { 60ll, 7 },
    { 210ll, 2 },
    { 420ll, 5 },
    { 504ll, 2 },
    { 840ll, 5 },
    { 1980ll, 2 },
    { 2520ll, 12 },
    { 3960ll, 2 },
    { 21840ll, 2 },
    { 27720ll, 14 },
    { 60060ll, 3 },
    { 180180ll, 2 },
    { 232560ll, 3 },
    { 240240ll, 2 },
    { 360360ll, 23 },
    { 371280ll, 2 },
    { 720720ll, 9 },
    { 1062600ll, 2 },
    { 1113840ll, 2 },
    { 1627920ll, 5 },
    { 1861860ll, 2 },
    { 4084080ll, 2 },
    { 7125300ll, 2 },
    { 12252240ll, 20 },
    { 13813800ll, 2 },
    { 14250600ll, 2 },
    { 17907120ll, 3 },
    { 19477920ll, 2 },
    { 21162960ll, 6 },
    { 58433760ll, 2 },
    { 124324200ll, 5 },
    { 232792560ll, 46 },
    { 300383160ll, 2 },
    { 327763800ll, 2 },
    { 411863760ll, 6 },
    { 495593280ll, 2 },
    { 564859680ll, 3 },
    { 944241480ll, 3 },
    { 1694579040ll, 2 },
    { 1767074400ll, 2 },
    { 2059318800ll, 3 },
    { 2162049120ll, 2 },
    { 2362159800ll, 4 },
    { 2703448440ll, 2 },
    { 3605401800ll, 6 },
    { 5083737120ll, 3 },
    { 5354228880ll, 26 },
    { 5451526080ll, 2 },
    { 16052105160ll, 2 },
    { 17226649440ll, 2 },
    { 18039122640ll, 2 },
    { 19437818400ll, 2 },
    { 26771144400ll, 29 },
    { 40156716600ll, 2 },
    { 40602383640ll, 2 },
    { 41078933280ll, 2 },
    { 62699424480ll, 2 },
    { 66088582560ll, 3 },
    { 68183715600ll, 2 },
    { 68502634200ll, 4 },
    { 75522807360ll, 2 },
    { 80313433200ll, 32 },
    { 90195613200ll, 2 },
    { 105657718320ll, 2 },
    { 111767455800ll, 3 },
    { 143282767320ll, 2 },
    { 176573156760ll, 2 },
    { 330442912800ll, 6 },
    { 365252247360ll, 2 },
    { 446626220040ll, 5 },
    { 447069823200ll, 7 },
    { 505009751400ll, 2 },
    { 534026132640ll, 4 },
    { 679895175960ll, 2 },
    { 706292627040ll, 2 },
    { 739604028240ll, 2 },
    { 1164544781400ll, 2 },
    { 1191289065120ll, 2 },
    { 1239326303040ll, 2 },
    { 1533325424400ll, 2 },
    { 1743805823760ll, 2 },
    { 1862675975160ll, 2 },
    { 2123581660200ll, 2 },
    { 2230550922600ll, 2 },
    { 2329089562800ll, 32 },
    { 2341207028160ll, 2 },
    { 3204634633200ll, 2 },
    { 3698020141200ll, 2 },
    { 3803928503760ll, 2 },
    { 5112221000040ll, 2 },
    { 5362119322560ll, 3 },
    { 6209288205120ll, 2 },
    { 6565126768200ll, 2 },
    { 6700599687600ll, 2 },
    { 7592645740680ll, 5 },
    { 7600186994400ll, 12 },
    { 8494326640800ll, 4 },
    { 10272403060920ll, 3 },
    { 12226387773600ll, 2 },
    { 15486757846560ll, 4 },
    { 15637589047080ll, 2 },
    { 16111241939520ll, 2 },
    { 19933230517200ll, 2 },
    { 21895071438240ll, 4 },
    { 30370582962720ll, 3 },
    { 31665491577720ll, 2 },
    { 33132310651440ll, 2 },
    { 35640326882160ll, 2 },
    { 46460273539680ll, 5 },
    { 62866342400400ll, 2 },
    { 71657610814440ll, 2 },
    { 72201776446800ll, 16 },
    { 73706596563600ll, 7 },
    { 94109334031440ll, 2 },
    { 131487468512400ll, 2 },
    { 142813628717760ll, 4 },
    { 144403552893600ll, 91 },
    { 158327457888600ll, 2 },
    { 165661553257200ll, 2 },
    { 166225698999360ll, 2 },
    { 174630852035640ll, 2 },
    { 177223661334720ll, 4 },
    { 216363439492200ll, 2 },
    { 232301367698400ll, 6 },
    { 270947713002120ll, 2 },
    { 281206918792800ll, 4 },
    { 391434710546880ll, 2 },
    { 429843920924880ll, 2 },
    { 482802943863240ll, 3 },
    { 484765260459480ll, 2 },
    { 518645014687800ll, 2 },
    { 531670984004160ll, 2 },
    { 623171140431840ll, 2 },
    { 634957071708960ll, 4 },
    { 655716348246960ll, 2 },
    { 788233718958840ll, 2 },
    { 817262451205200ll, 4 },
    { 856154997807480ll, 2 },
    { 865453757968800ll, 2 },
    { 941488071844320ll, 6 },
    { 965605887726480ll, 3 },
    { 1037290029375600ll, 2 },
    { 1056461217411600ll, 2 },
    { 1108292205220200ll, 2 },
    { 1200950228016000ll, 2 },
    { 1318692094639920ll, 2 },
    { 1400425334708400ll, 3 },
    { 1601780734932840ll, 2 },
    { 1904871215126880ll, 4 },
    { 2126128883266080ll, 2 },
    { 2216584410440400ll, 2 },
    { 3115855702159200ll, 2 },
    { 3169383652234800ll, 6 },
    { 3203561469865680ll, 2 },
    { 3264337164488400ll, 2 },
    { 3482045359163280ll, 2 },
    { 3522320269027560ll, 2 },
    { 5342931457063200ll, 88 },
    { 5390986209008400ll, 2 },
    { 6129477470516400ll, 2 },
    { 6759241214085360ll, 3 },
    { 7706294302707000ll, 2 },
    { 8167034497572720ll, 2 },
    { 8207650045675080ll, 2 },
    { 8426004094347840ll, 2 },
    { 9335610264380400ll, 2 },
    { 9524356075634400ll, 4 },
};

void testLcmCounter() {
    //return; //TODO: if you want to test, make this line a comment.

    cout << "--- LCM Counter -------------------------" << endl;
    {
        LcmCounter counter(MAXR);

        for (int i = 0; i < sizeof(sLcmCount) / sizeof(sLcmCount[0]); i++) {
            auto ans = counter.countLcm(sLcmCount[i].first, MAXR);
            if (ans != sLcmCount[i].second)
                cout << "Mismatched at " << sLcmCount[i].first << " : " << ans << ", " << sLcmCount[i].second << endl;
            assert(ans == sLcmCount[i].second);
        }
    }

    cout << "OK!" << endl;
}
