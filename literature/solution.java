import java.util.*;

public class RockyMine {
    private static final int[] dx = new int[] {1,0,-1,0}, dy = new int[] {0,1,0,-1};
    private static final char[] dir = new char[] {'E','S','W','N'};
    private int maxMoves, w, h, goldCut;
    private int[] dynamite, rocks, gold, dlim;
    private int[][] effect;
    private int[][][] ceffect;
    private final int[] currPoints = new int[10240], reachPoint = new int[10240];
    private final int[] dpts = new int[24];
    private final boolean[] allPoints = new boolean[25600];
    static long TIMEOUT = 9500;
    private long start = System.currentTimeMillis();
    private long seed = 9;
    private final long multiplier = 0x5DEECE66DL, addend = 0xBL, mask = (1L << 48) - 1;
    private final int[] distAux = new int[1 << 20];

    public String collectGold(int[] DYNAMITE, int[] EFFECT, int W, int[] GOLD, int[] ROCKS, int MAXMOVES) {
        maxMoves = MAXMOVES;
        dynamite = new int[DYNAMITE.length];
        System.arraycopy(DYNAMITE, 0, dynamite, 0, dynamite.length);
        w = W;
        h = ROCKS.length / w;
        rocks = new int[h << 8];
        gold = new int[h << 8];
        for (int i = 0; i < h; i++) {
            System.arraycopy(GOLD, i * w, gold, i << 8, w);
            System.arraycopy(ROCKS, i * w, rocks, i << 8, w);
        }
        effect = new int[dynamite.length][25];
        int p = 0;
        for (int i = 0; i < dynamite.length; i++) {
            for (int j = 0; j < 5; j++) {
                for (int k = 0; k < 5; k++) {
                    effect[i][j * 5 + k] = EFFECT[p++];
                }
            }
        }
        ceffect = new int[dynamite.length][dynamite.length][25];
        for (int i = 0; i < dynamite.length; i++) {
            int[] ei = effect[i];
            for (int j = 0; j < dynamite.length; j++) {
                int[] cij = ceffect[i][j];
                int[] ej = effect[j];
                for (int k = 0; k < 25; k++) {
                    cij[k] = ei[k] + ej[k];
                }
            }
        }

        int totBombs = 0;
        for (int i = 0; i < dynamite.length; i++) {
            totBombs += dynamite[i];
        }
        int parts = (maxMoves + 199) / 200;
        if (parts > 32) parts = 32;
        Solution best = new Solution(0, "", 0);
        best.save(dynamite, rocks, gold, maxMoves);

        dlim = new int[dynamite.length];
        for (int i = 0; i < parts; i++) {
            seed = 9 + i;
            Solution partBest = null;
            int cut = MAXMOVES * (parts - i - 1) / parts;
            OUT: for (int step = 0; step < 10000; step++) {
                if (totBombs * 8 < MAXMOVES || nextInt(totBombs * 17 / MAXMOVES) == 1) {
                    for (int j = 0; j < dynamite.length; j++) {
                        dlim[j] = Math.max(0, DYNAMITE[j] * (parts - i - 1) / parts);
                    }
                } else {
                    Arrays.fill(dlim, 0);
                }

                goldCut = (step == 0) ? 0 : (step % (parts > 1 ? 5 : 10) - 1);
                if (goldCut < 0) goldCut = 0;
                for (int factor = 1100; factor >= 500; factor -= 200) {
                    for (int mode = 5; mode >= 2; mode--) {
                        maxMoves = best.restore(dynamite, rocks, gold);
                        Solution s = walk(best, cut, mode, factor, step);
                        if (partBest == null || s.val * partBest.path.length() > partBest.val * s.path.length()) {
                            partBest = s;
                            partBest.save(dynamite, rocks, gold, maxMoves);
                        }
                        if (System.currentTimeMillis() - start > TIMEOUT * (i + 1) / parts) break OUT;
                    }
                }
            }
            best = partBest;
        }
        return best.path;
    }

    private Solution walk(Solution init, int cut, int mode, int factor, int step) {
        StringBuilder path = new StringBuilder();
        path.append(init.path);
        int pgold = init.val;
        int pc = init.position;

        int totBombs = 0;
        for (int i = 0; i < dynamite.length; i++) {
            totBombs += dynamite[i];
        }
        int maxDist = step == 0 ? 20 : (nextInt(16) + 10);
        int[] points = new int[1000];
        int[] bomb = new int[dynamite.length];
        int[] aux = new int[dynamite.length];
        int ss = mode == 0 ? 1 : (1 + nextInt(5));
        while (maxMoves > cut) {
            int nreach = updateReachable(pc, maxDist);
            double maxScore = -1e10;
            int dest = -1;
            int numPoints = 0;
            for (int pr = 0; pr < nreach; pr++) {
                int p = reachPoint[pr];
                int dp = p >>> 16;
                p &= 65535;
                if (dp == 0) continue;
                int pg = gold[p];
                if (pg > goldCut) points[numPoints++] = p;
                if (maxMoves > 5 && totBombs > 0) {
                    double score = 0;
                    if (step > 0) score += 500 - nextInt(1001);
                    double div = ss + dp;
                    if (pg > goldCut) score += 10000.0 * pg / div;
                    double min = (maxScore - score) * div;
                    score += evalBombing(p, aux, maxMoves > 6, mode, factor, min) / div;
                    if (score > maxScore) {
                        maxScore = score;
                        dest = p;
                        System.arraycopy(aux, 0, bomb, 0, aux.length);
                    }
                }
            }
            if (dest >= 0) points[numPoints++] = dest;
            if (numPoints == 0) {
                if (maxDist < 20) {
                    maxDist += 4;
                    continue;
                }
                path.append("-");
                maxMoves--;
                continue;
            }
            int[] p = getOrder(pc, points, numPoints, dest);
            int idx = 0;
            while (maxMoves > 0 && idx < p.length) {
                int pn = p[idx];
                if (pn < 0) break;
                int cd = distance(pc, pn);
                int bnp = -1;
                int cdir = 0;
                int pcx = pc & 255;
                int pcy = (pc >>> 8) & 255;
                for (int i = 0; i < dx.length; i++) {
                    int nx = pcx + dx[i];
                    if (nx < 0 || nx >= w) continue;
                    int ny = pcy + dy[i];
                    if (ny < 0 || ny >= h) continue;
                    int np = (ny << 8) | nx;
                    if (rocks[np] > 0) continue;
                    int nd = distance(np, pn);
                    if (nd < cd) {
                        if (bnp == -1 || gold[bnp] < gold[np]) {
                            bnp = np;
                            cdir = i;
                        }
                    }
                }
                path.append(dir[cdir]);
                maxMoves--;
                pc = bnp;
                pgold += gold[pc];
                gold[pc] = 0;
                if (cd == 1) idx++;
                if (pc == dest) {
                    if (maxMoves <= 5) break;
                    int nb = 0;
                    for (int i = 0; i < bomb.length; i++) {
                        nb += bomb[i];
                    }
                    nreach = updateReachable(pc, 5);
                    int mg = goldCut;
                    int esc = -1;
                    for (int pr = 0; pr < nreach; pr++) {
                        int ps = reachPoint[pr];
                        if ((ps >>> 16) != 5) continue;
                        ps &= 65535;
                        if (Math.max(Math.abs((ps & 255) - (pc & 255)), Math.abs(((ps >>> 8) & 255) - ((pc >>> 8) & 255))) <= 1 + nb) continue;
                        int psg = gold[ps];
                        if (psg > mg) {
                            mg = psg;
                            esc = ps;
                        }
                    }
                    if (esc == -1) {
                        for (int pr = 0; pr < nreach; pr++) {
                            int ps = reachPoint[pr];
                            if ((ps >>> 16) != 5) continue;
                            ps &= 65535;
                            if (Math.max(Math.abs((ps & 255) - (pc & 255)), Math.abs(((ps >>> 8) & 255) - ((pc >>> 8) & 255))) <= 1 + nb) continue;
                            int psg = 0;
                            for (int i = 0; i < 4; i++) {
                                int nx = (ps & 255) + dx[i];
                                if (nx < 0 || nx >= w) continue;
                                int ny = (ps >>> 8) + dy[i];
                                if (ny < 0 || ny >= h) continue;
                                int np = (ny << 8) | nx;
                                if (rocks[np] > 0) continue;
                                psg += gold[np];
                            }
                            if (psg > mg) {
                                mg = psg;
                                esc = ps;
                            }
                        }
                    }
                    StringBuilder escPath = new StringBuilder();
                    int pb = pc;
                    if (esc >= 0) {
                        while (pc != esc) {
                            cd = distance(pc, esc);
                            bnp = -1;
                            cdir = 0;
                            pcx = pc & 255;
                            pcy = (pc >>> 8) & 255;
                            for (int i = 0; i < dx.length; i++) {
                                int nx = pcx + dx[i];
                                if (nx < 0 || nx >= w) continue;
                                int ny = pcy + dy[i];
                                if (ny < 0 || ny >= h) continue;
                                int np = (ny << 8) | nx;
                                if (rocks[np] > 0) continue;
                                int nd = distance(np, esc);
                                if (nd < cd) {
                                    if (bnp == -1 || gold[bnp] < gold[np]) {
                                        bnp = np;
                                        cdir = i;
                                    }
                                }
                            }
                            escPath.append(dir[cdir]);
                            maxMoves--;
                            pc = bnp;
                            pgold += gold[pc];
                            gold[pc] = 0;
                        }
                    }
                    if (esc < 0 && maxMoves >= 7) {
                        escPath.delete(0, escPath.length());
                        pcx = pc & 255;
                        pcy = (pc >>> 8) & 255;
                        for (int i = 0; i < dx.length; i++) {
                            int nx = pcx + dx[i];
                            if (nx < 0 || nx >= w) continue;
                            int ny = pcy + dy[i];
                            if (ny < 0 || ny >= h) continue;
                            int np = (ny << 8) | nx;
                            if (rocks[np] > 0) continue;
                            if (gold[np] > 0) {
                                escPath.append(dir[i]).append(dir[(i + 2) % 4]);
                                maxMoves -= 2;
                                pgold += gold[np];
                                gold[np] = 0;
                                esc = 0;
                                if (escPath.length() == 4) break;
                            }
                        }
                        if (esc < 0) {
                            OUT: for (int i = 0; i < dx.length; i++) {
                                int nx = pcx + dx[i];
                                if (nx < 0 || nx >= w) continue;
                                int ny = pcy + dy[i];
                                if (ny < 0 || ny >= h) continue;
                                int np = (ny << 8) | nx;
                                if (rocks[np] > 0) continue;
                                for (int j = 0; j < dx.length; j++) {
                                    int mx = nx + dx[j];
                                    if (mx < 0 || mx >= w) continue;
                                    int my = ny + dy[j];
                                    if (my < 0 || my >= h) continue;
                                    int mp = (my << 8) | mx;
                                    if (rocks[mp] > 0) continue;
                                    if (gold[mp] > 0) {
                                        escPath.append(dir[i]).append(dir[j]).append(dir[(j + 2) % 4]).append(dir[(i + 2) % 4]);
                                        maxMoves -= 4;
                                        pgold += gold[mp];
                                        gold[mp] = 0;
                                        esc = 0;
                                        break OUT;
                                    }
                                }
                            }
                        }
                    }
                    boolean safe1 = true;
                    boolean safe2 = true;
                    boolean safe3 = true;
                    NEXT: for (int i = 0; i < bomb.length; i++) {
                        if (bomb[i] > 0) {
                            if (safe1 && effect[i][7] > 0) safe1 = false;
                            if (safe2 && (effect[i][2] > 0 || effect[i][6] > 0)) safe2 = false;
                            if (safe3 && effect[i][1] > 0) safe3 = false;
                            for (int j = 0; j < bomb[i]; j++) {
                                path.append(i);
                                applyBomb(pb, i);
                                maxMoves--;
                                totBombs--;
                                if (maxMoves <= 5) break NEXT;
                            }
                        }
                    }
                    if (esc >= 0) {
                        int m = 5;
                        if (safe1) m--;
                        while (escPath.length() < m) {
                            escPath.append("-");
                            maxMoves--;
                        }
                        path.append(escPath);
                    } else {
                        if (!safe1) {
                            path.append("---");
                            maxMoves -= 3;
                        } else if (!safe2) {
                            path.append("--");
                            maxMoves -= 2;
                        } else if (!safe3) {
                            path.append("-");
                            maxMoves -= 1;
                        }
                        path.append("--");
                        maxMoves -= 2;
                    }
                    break;
                }
            }
        }
        return new Solution(pgold, path.toString(), pc);
    }

    private int updateReachable(int p1, int maxDist) {
        int nr = 0;
        allPoints[p1] = true;
        int dist = 0;
        int ds = 0;
        currPoints[0] = p1 & 65535;
        int pc = 0;
        int pn = 1;
        int np = 0;
        while (pc < pn && dist < maxDist) {
            dist++;
            ds = dist << 16;
            int pv = pn;
            for (int j = pc; j < pn; j++) {
                int p = currPoints[j];
                int px = p & 255;
                if (px < w - 1 && rocks[np = p + 1] == 0 && !allPoints[np]) {
                    reachPoint[nr++] = ds | np;
                    allPoints[currPoints[pv++] = np] = true;
                }
                int py = p >>> 8;
                if (py < h - 1 && rocks[np = p + 256] == 0 && !allPoints[np]) {
                    reachPoint[nr++] = ds | np;
                    allPoints[currPoints[pv++] = np] = true;
                }
                if (px > 0 && rocks[np = p - 1] == 0 && !allPoints[np]) {
                    reachPoint[nr++] = ds | np;
                    allPoints[currPoints[pv++] = np] = true;
                }
                if (py > 0 && rocks[np = p - 256] == 0 && !allPoints[np]) {
                    reachPoint[nr++] = ds | np;
                    allPoints[currPoints[pv++] = np] = true;
                }
            }
            pc = pn;
            pn = pv;
        }
        for (int k = 0; k < pn; k++) {
            allPoints[currPoints[k]] = false;
        }
        return nr;
    }

    private int[] getOrder(Integer pc, int[] points, int n, int dest) {
        for (int i = 0; i < n; i++) {
            int p1 = points[i];
            for (int j = i + 1; j < n; j++) {
                distAux[(i << 10) | j] = distAux[(j << 10) | i] = distance(p1, points[j]);
            }
        }
        int[] order = new int[n];
        boolean[] used = new boolean[n];
        int tot = 0;
        for (int i = 1; i < n; i++) {
            int min = Integer.MAX_VALUE;
            int next = -1;
            int base = order[i - 1] << 10;
            for (int j = 1; j < n; j++) {
                if (used[j]) continue;
                int da = distAux[base | j];
                if (da < min) {
                    min = da;
                    next = j;
                }
            }
            tot += min;
            used[order[i] = next] = true;
        }

        while (true) {
            boolean imp = false;
            for (int i = 2; i < n - 1; i++) {
                int b1 = order[i - 2] << 10;
                int b2 = order[i + 1] << 10;
                int d1 = distAux[b1 | order[i - 1]] + distAux[b2 | order[i]];
                int d2 = distAux[b1 | order[i]] + distAux[b2 | order[i - 1]];
                if (d2 < d1) {
                    tot -= d1 - d2;
                    imp = true;
                    int aux = order[i - 1];
                    order[i - 1] = order[i];
                    order[i] = aux;
                }
            }
            if (!imp) break;
        }

        int s = 0;
        for (int i = 0; i < n; i++) {
            s++;
            if (points[order[i]] == dest) break;
        }
        int[] ret = new int[s];
        for (int i = 0; i < s; i++) {
            ret[i] = points[order[i]];
        }
        return ret;
    }

    private void applyBomb(int p, int bomb) {
        dynamite[bomb]--;
        int[] eb = effect[bomb];
        int px = p & 255;
        int py = (p >>> 8) & 255;
        for (int i = 0; i < 5; i++) {
            int ry = py + i - 2;
            if (ry < 0 || ry >= h) continue;
            for (int j = 0; j < 5; j++) {
                if (i == 2 && j == 2) continue;
                int rx = px + j - 2;
                if (rx < 0 || rx >= w) continue;
                int rr = rx | (ry << 8);
                int pr = rocks[rr];
                int edc = eb[i * 5 + j];
                if (edc == 0) continue;
                int nr = pr - edc;
                if (nr < 0) {
                    nr = 0;
                    gold[rr] = 0;
                }
                rocks[rr] = nr;
            }
        }
    }

    private double evalBombing(int p, int[] bomb, boolean combo2, int mode, int factor, double min) {
        int id1 = -1;
        int id2 = -1;
        int best = 0;
        int np = 0;
        int px = p & 255;
        int py = (p >>> 8) & 255;
        int tg = 0;

        int pr = 0;
        int vg = 0;
        int rr = 0;
        if (py > 1) {
            if (px > 1 && (vg = gold[rr = p - 514]) > goldCut && (pr = rocks[rr]) < 9) {
                tg += vg;
                dpts[np++] = (0 << 16) | (pr << 8) | vg;
            }
            if (px > 0 && (vg = gold[rr = p - 513]) > goldCut && (pr = rocks[rr]) < 9) {
                tg += vg;
                dpts[np++] = (1 << 16) | (pr << 8) | vg;
            }
            if ((vg = gold[rr = p - 512]) > goldCut && (pr = rocks[rr]) < 9) {
                tg += vg;
                dpts[np++] = (2 << 16) | (pr << 8) | vg;
            }
            if (px < w - 1 && (vg = gold[rr = p - 511]) > goldCut && (pr = rocks[rr]) < 9) {
                tg += vg;
                dpts[np++] = (3 << 16) | (pr << 8) | vg;
            }
            if (px < w - 2 && (vg = gold[rr = p - 510]) > goldCut && (pr = rocks[rr]) < 9) {
                tg += vg;
                dpts[np++] = (4 << 16) | (pr << 8) | vg;
            }
        }
        if (py > 0) {
            if (px > 1 && (vg = gold[rr = p - 258]) > goldCut && (pr = rocks[rr]) < 9) {
                tg += vg;
                dpts[np++] = (5 << 16) | (pr << 8) | vg;
            }
            if (px > 0 && (vg = gold[rr = p - 257]) > goldCut && (pr = rocks[rr]) < 9) {
                tg += vg;
                dpts[np++] = (6 << 16) | (pr << 8) | vg;
            }
            if ((vg = gold[rr = p - 256]) > goldCut && (pr = rocks[rr]) < 9) {
                tg += vg;
                dpts[np++] = (7 << 16) | (pr << 8) | vg;
            }
            if (px < w - 1 && (vg = gold[rr = p - 255]) > goldCut && (pr = rocks[rr]) < 9) {
                tg += vg;
                dpts[np++] = (8 << 16) | (pr << 8) | vg;
            }
            if (px < w - 2 && (vg = gold[rr = p - 254]) > goldCut && (pr = rocks[rr]) < 9) {
                tg += vg;
                dpts[np++] = (9 << 16) | (pr << 8) | vg;
            }
        }
        if (px > 1 && (vg = gold[rr = p - 2]) > goldCut && (pr = rocks[rr]) < 9) {
            tg += vg;
            dpts[np++] = (10 << 16) | (pr << 8) | vg;
        }
        if (px > 0 && (vg = gold[rr = p - 1]) > goldCut && (pr = rocks[rr]) < 9) {
            tg += vg;
            dpts[np++] = (11 << 16) | (pr << 8) | vg;
        }
        if (px < w - 1 && (vg = gold[rr = p + 1]) > goldCut && (pr = rocks[rr]) < 9) {
            tg += vg;
            dpts[np++] = (13 << 16) | (pr << 8) | vg;
        }
        if (px < w - 2 && (vg = gold[rr = p + 2]) > goldCut && (pr = rocks[rr]) < 9) {
            tg += vg;
            dpts[np++] = (14 << 16) | (pr << 8) | vg;
        }
        if (py < h - 1) {
            if (px > 1 && (vg = gold[rr = p + 254]) > goldCut && (pr = rocks[rr]) < 9) {
                tg += vg;
                dpts[np++] = (15 << 16) | (pr << 8) | vg;
            }
            if (px > 0 && (vg = gold[rr = p + 255]) > goldCut && (pr = rocks[rr]) < 9) {
                tg += vg;
                dpts[np++] = (16 << 16) | (pr << 8) | vg;
            }
            if ((vg = gold[rr = p + 256]) > goldCut && (pr = rocks[rr]) < 9) {
                tg += vg;
                dpts[np++] = (17 << 16) | (pr << 8) | vg;
            }
            if (px < w - 1 && (vg = gold[rr = p + 257]) > goldCut && (pr = rocks[rr]) < 9) {
                tg += vg;
                dpts[np++] = (18 << 16) | (pr << 8) | vg;
            }
            if (px < w - 2 && (vg = gold[rr = p + 258]) > goldCut && (pr = rocks[rr]) < 9) {
                tg += vg;
                dpts[np++] = (19 << 16) | (pr << 8) | vg;
            }
        }
        if (py < h - 2) {
            if (px > 1 && (vg = gold[rr = p + 510]) > goldCut && (pr = rocks[rr]) < 9) {
                tg += vg;
                dpts[np++] = (20 << 16) | (pr << 8) | vg;
            }
            if (px > 0 && (vg = gold[rr = p + 511]) > goldCut && (pr = rocks[rr]) < 9) {
                tg += vg;
                dpts[np++] = (21 << 16) | (pr << 8) | vg;
            }
            if ((vg = gold[rr = p + 512]) > goldCut && (pr = rocks[rr]) < 9) {
                tg += vg;
                dpts[np++] = (22 << 16) | (pr << 8) | vg;
            }
            if (px < w - 1 && (vg = gold[rr = p + 513]) > goldCut && (pr = rocks[rr]) < 9) {
                tg += vg;
                dpts[np++] = (23 << 16) | (pr << 8) | vg;
            }
            if (px < w - 2 && (vg = gold[rr = p + 514]) > goldCut && (pr = rocks[rr]) < 9) {
                tg += vg;
                dpts[np++] = (24 << 16) | (pr << 8) | vg;
            }
        }

        if (tg * 1000 <= min) return -1e90;

        for (int d1 = 0; d1 < dynamite.length; d1++) {
            int dd1 = dynamite[d1];
            if (dd1 > dlim[d1]) {
                int[] ed1 = effect[d1];
                int sum = 0;
                for (int ii = 0; ii < np; ii++) {
                    int dpi = dpts[ii];
                    int edc = ed1[dpi >>> 16];
                    if (edc == 0) continue;
                    int nr = ((dpi >>> 8) & 255) - edc;
                    if (nr < 0) sum -= (dpi & 255) * factor;
                    else if (nr == 0) sum += (dpi & 255) * 1000;
                }
                if (sum > best) {
                    best = sum;
                    id1 = d1;
                    id2 = -1;
                }
                if (!combo2) continue;
                int[][] ce1 = ceffect[d1];
                for (int d2 = d1; d2 < dynamite.length; d2++) {
                    if (dynamite[d2] > dlim[d2] && (d1 != d2 || dynamite[d2] > dlim[d2] + 1)) {
                        int[] ed12 = ce1[d2];
                        sum = 0;
                        for (int ii = 0; ii < np; ii++) {
                            int dpi = dpts[ii];
                            int idx = dpi >>> 16;
                            int edc = ed12[idx];
                            if (edc == 0) continue;
                            int nr = ((dpi >>> 8) & 255) - edc;
                            if (nr < 0) sum -= (dpi & 255) * factor;
                            else if (nr == 0) sum += (dpi & 255) * 1000;
                        }
                        if (sum > 0) {
                            if (sum * mode > best * 5) {
                                best = sum * mode / 5;
                                id1 = d1;
                                id2 = d2;
                            }
                        }
                    }
                }
            }
        }
        if (id1 < 0) return -1e100;
        if (bomb != null) {
            if (best > min) {
                Arrays.fill(bomb, 0);
                if (id1 >= 0) bomb[id1]++;
                if (id2 >= 0) bomb[id2]++;
            }
        }
        return best;
    }

    private int distance(int p1, int p2) {
        if (p1 == p2) return 0;
        int p1x = p1 & 255;
        int p1y = (p1 >>> 8) & 255;
        int p2x = p2 & 255;
        int p2y = (p2 >>> 8) & 255;
        int kx = p2x - p1x;
        int ky = p2y - p1y;
        int ax = p1x;
        int ay = p1y;
        boolean bad = false;
        while (ax != p2x) {
            ax += kx > 0 ? 1 : -1;
            if (rocks[(ay << 8) | ax] > 0) {
                bad = true;
                break;
            }
        }
        if (!bad) {
            while (ay != p2y) {
                ay += ky > 0 ? 1 : -1;
                if (rocks[(ay << 8) | ax] > 0) {
                    bad = true;
                    break;
                }
            }
            if (!bad) return Math.abs(kx) + Math.abs(ky);
        }
        ax = p1x;
        ay = p1y;
        bad = false;
        while (ay != p2y) {
            ay += ky > 0 ? 1 : -1;
            if (rocks[(ay << 8) | ax] > 0) {
                bad = true;
                break;
            }
        }
        if (!bad) {
            while (ax != p2x) {
                ax += kx > 0 ? 1 : -1;
                if (rocks[(ay << 8) | ax] > 0) {
                    bad = true;
                    break;
                }
            }
            if (!bad) return Math.abs(kx) + Math.abs(ky);
        }

        allPoints[p1] = true;
        int dist = 0;
        currPoints[0] = p1 & 65535;
        int pc = 0;
        int pn = 1;
        int np = 0;
        OUT: while (pc < pn) {
            dist++;
            int pv = pn;
            for (int j = pc; j < pn; j++) {
                int p = currPoints[j];
                int px = p & 255;
                if (px < w - 1 && rocks[np = p + 1] == 0 && !allPoints[np]) {
                    if (np == p2) {
                        pn = pv;
                        break OUT;
                    }
                    allPoints[currPoints[pv++] = np] = true;
                }
                int py = p >>> 8;
                if (py < h - 1 && rocks[np = p + 256] == 0 && !allPoints[np]) {
                    if (np == p2) {
                        pn = pv;
                        break OUT;
                    }
                    allPoints[currPoints[pv++] = np] = true;
                }
                if (px > 0 && rocks[np = p - 1] == 0 && !allPoints[np]) {
                    if (np == p2) {
                        pn = pv;
                        break OUT;
                    }
                    allPoints[currPoints[pv++] = np] = true;
                }
                if (py > 0 && rocks[np = p - 256] == 0 && !allPoints[np]) {
                    if (np == p2) {
                        pn = pv;
                        break OUT;
                    }
                    allPoints[currPoints[pv++] = np] = true;
                }
            }
            pc = pn;
            pn = pv;
        }
        for (int k = 0; k < pn; k++) {
            allPoints[currPoints[k]] = false;
        }
        return dist;
    }

    private final int nextInt(int n) {
        if ((n & -n) == n) {
            return (int) ((n * (long) ((int) ((seed = (seed * multiplier + addend) & mask) >>> 17))) >> 31);
        }
        int bits, val;
        do {
            bits = (int) ((seed = (seed * multiplier + addend) & mask) >>> 17);
            val = bits % n;
        } while (bits - val + (n - 1) < 0);
        return val;
    }
}

class Solution {
    public int val, position, maxMoves;
    public String path;
    public int[] dynamite, rocks, gold;

    public void save(int[] dynamite, int[] rocks, int[] gold, int maxMoves) {
        this.dynamite = new int[dynamite.length];
        this.rocks = new int[rocks.length];
        this.gold = new int[gold.length];
        System.arraycopy(dynamite, 0, this.dynamite, 0, dynamite.length);
        System.arraycopy(rocks, 0, this.rocks, 0, rocks.length);
        System.arraycopy(gold, 0, this.gold, 0, gold.length);
        this.maxMoves = maxMoves;
    }

    public int restore(int[] dynamite, int[] rocks, int[] gold) {
        System.arraycopy(this.dynamite, 0, dynamite, 0, dynamite.length);
        System.arraycopy(this.rocks, 0, rocks, 0, rocks.length);
        System.arraycopy(this.gold, 0, gold, 0, gold.length);
        return maxMoves;
    }

    public Solution(int val, String path, int position) {
        this.val = val;
        this.path = path;
        this.position = position;
    }
}