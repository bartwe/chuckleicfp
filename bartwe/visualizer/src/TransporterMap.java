public class TransporterMap {
    byte[] mapping = new byte[9];

    public byte getTargetTransporter(byte sourceTransporter) {
        assert Cell.isTransporter(sourceTransporter);
        assert mapping[sourceTransporter-Cell.TrampolineSource1] != 0;
        return mapping[sourceTransporter-Cell.TrampolineSource1];
    }

    public void addMapping(byte source, byte target) {
        assert Cell.isTransporter(source);
        assert Cell.TrampolineTarget1 <= target && target <= Cell.TrampolineTarget9;
        assert mapping[source-Cell.TrampolineSource1] == 0;
        mapping[source-Cell.TrampolineSource1] = target;
    }

    boolean[] cleanupListTemp = new boolean[9];
    public boolean[] cleanupList(byte target) {
        for (int i = 0; i< mapping.length; i++)
            cleanupListTemp[i] = (mapping[i] == target);
        return cleanupListTemp;
    }
}
